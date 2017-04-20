import httplib
import argparse
import json
from pprint import pprint
from collections import OrderedDict
import importlib

PROTOCOL = 'http'
HOST = ''
PORT = 0
TIMEOUT = 0

def parse_config(config_filename):
    '''
    parse configuration file
    '''
    global PROTOCOL, HOST, PORT, TIMEOUT
    with open(config_filename, 'r') as cf:
        data = json.load(cf)
    # pprint(data)
    PROTOCOL = data["PROTOCOL"]
    HOST = data["HOST"]
    PORT = data["PORT"]
    TIMEOUT = data["TIMEOUT"]

def connection(protocol, host, port, timeout):
    '''
    make connection, return connection instance
    currently ONLY support http
    '''
    # validations
    if protocol != 'http':
        print('ERROR: protocol {} not supported'.format(protocol))
        exit(0)

    if not host:
        print('ERROR: missing host address')
        exit(0)
    
    if not port:
        print('WARN: port is not specified, use default port 80')
        port = 80
    else:
        try:
            port = int(port)
        except:
            print('ERROR: invalid port value {} in configuration'.format(port))
            exit(0)

    if timeout:
        try:
            timeout = int(timeout)
        except:
            print('ERROR: invalid timeout value {} in configuration'.format(timeout))
            exit(0)

    # establish connection
    print('opening {} connection on host ({}) port ({})'.format(protocol, host, port))
    try:
        if timeout:
            conn = httplib.HTTPConnection(host, port, timeout=timeout)
        else:
            conn = httplib.HTTPConnection(host, port)
        return conn
    except:
        print('ERROR: failed to open connection')
        exit(0)

def run_case(conn, module, case, id=None):
    '''
    run single case
    return True if case passed, False otherwise
    '''
    name = case.get('NAME')
    
    qpath = case.get('REQ_PATH')
    qmethod = case.get('REQ_METHOD')
    qparams = case.get('REQ_PARAMS')
    qfile = case.get('REQ_FILE')
    qheaders = case.get('REQ_HEADERS')

    rstatus = case.get('RESP_STATUS')
    rreason = case.get('RESP_REASON')
    rversion = case.get('RESP_VERSION')
    rheaders = case.get('RESP_HEADERS')
    rverifun = case.get('RESP_VERIFY_FUNCTION')

    print('-------------------------------------------')
    if not id is None:
        print('case id: \t\t{}'.format(id))
    if not name is None:
        print('case name: \t\t{}'.format(name))

    if not qpath:
        print('WARN: path is not specified, use default path: /')
        qpath = "/"
    print('request path: \t\t{}'.format(qpath))

    if not qmethod:
        print('WARN: method is not specified, use default method: GET')
        qmethod = "GET"
    if not qmethod in ["GET", "POST", "HEAD"]:
        print('ERROR: method {} not supported'.format(qmethod))
        return False
    print('request method: \t{}'.format(qmethod))

    # TODO: parse qheaders
    if qmethod == "POST":
        # TODO: parse qparams / qfile
        pass

    try:
        conn.request(qmethod, qpath)
    except:
        print('ERROR: request failed')
        return False

    resp = conn.getresponse()

    # verifications
    print('verifications...')
    ret = True

    if rstatus:
        try:
            rstatus = int(rstatus)
        except:
            print('ERROR: invalid resp_status value {}'.format(rstatus))
            return False
        print('\n - STATUS:')
        print('EXPECT: {}'.format(rstatus))
        print('ACTUAL: {}'.format(resp.status))
        if rstatus == resp.status:
            print('[OK]')
        else:
            print('[FAIL]')
            ret = False

    if rreason:
        try:
            rreason = str(rreason)
        except:
            print('ERROR: invalid resp_reason value {}'.format(rreason))
            return False
        print('\n - REASON:')
        print('EXPECT: {}'.format(rreason))
        print('ACTUAL: {}'.format(resp.reason))
        if rreason == resp.reason:
            print('[OK]')
        else:
            print('[FAIL]')
            ret = False

    # TODO: rversion

    # TODO: rheaders

    if rverifun:
        try:
            rverifun = str(rverifun)
            print('\n - VERIFICATION by {}'.format(rverifun))
            if getattr(module, rverifun)(resp.read()):
                print('[OK]')
            else:
                print('[FAIL]')
                ret = False
        except:
            print('ERROR: invalid verification function {}'.format(rverifun))
            return False

    if not ret:
        print('\ntest failed')
    return ret


def run_tests(conn, module, test_file):
    '''
    test runner
    '''
    cases = json.load(test_file, object_pairs_hook=OrderedDict)
    # pprint(cases)

    print('prepare to run {} test cases...'.format(len(cases)))
    nr_run = nr_success = 0
    for c in cases:
        if run_case(conn, module, cases[c], c):
            nr_success += 1
        nr_run += 1

    # summary
    print('\n\n===========================================')
    print('total:   {}'.format(nr_run))
    print('passed:  {}'.format(nr_success))
    print('failed:  {}'.format(nr_run - nr_success))


'''
===============================================================================
'''
def main(test_cases_file, lib, config):
    if not test_cases_file:
        print('ERROR: must specify test cases file')
        exit(0)
    print('test cases file: \t{}'.format(test_cases_file))

    module = None
    if lib:
        try:
            module = __import__(lib)
        except:
            print('ERROR: failed to import lib: {}'.format(lib))
            exit(0)
        print('verification lib: \t{}'.format(lib))
    if config:
        print('configuration file: \t{}'.format(config))

    # parse configurations
    parse_config(config)

    conn = connection(PROTOCOL, HOST, PORT, TIMEOUT)
    with open(test_cases_file, 'r') as tf:
        run_tests(conn, module, tf)

    conn.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'integration test framework (support HTTP only)')
    parser.add_argument('test_cases_file', type = str, help = 'json file with test cases')
    parser.add_argument('--lib', type = str, help = 'python file (under SAME DIRECTORY) with verification functions')
    parser.add_argument('--config', default = "config.json", type = str, help = 'json file with global configurations')

    args = vars(parser.parse_args())

    main(**args)