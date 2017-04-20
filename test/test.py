import httplib
import urllib
import argparse
import json
from pprint import pprint
from collections import OrderedDict, defaultdict
import importlib

PROTOCOL = 'http'
HOST = ''
PORT = 0
TIMEOUT = 0
COLOR = True

http_version_map = {10:'HTTP/1.0', 11:'HTTP/1.1'} # http version int to str

# utils
def printgreen(s):
    if COLOR:
        print('\x1b[6;30;42m' + s + '\x1b[0m')
    else:
        print(s)

def printhigh(s):
    if COLOR:
        print('\x1b[6;30;47m' + s + '\x1b[0m')
    else:
        print(s)

def printred(s):
    if COLOR:
        print('\x1b[6;30;41m' + s + '\x1b[0m')
    else:
        print(s)

def printerror(s):
    if COLOR:
        print('\x1b[6;30;43m' + s + '\x1b[0m')
    else:
        print(s)

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
        printerror('ERROR: protocol {} not supported'.format(protocol))
        exit(0)

    if not host:
        printerror('ERROR: missing host address')
        exit(0)
    
    if not port:
        print('WARN: port is not specified, use default port 80')
        port = 80
    else:
        try:
            port = int(port)
        except:
            printerror('ERROR: invalid port value {} in configuration'.format(port))
            exit(0)

    if timeout:
        try:
            timeout = int(timeout)
        except:
            printerror('ERROR: invalid timeout value {} in configuration'.format(timeout))
            exit(0)

    # establish connection
    print('opening {} connection on host ({}) port ({})'.format(protocol, host, port))
    try:
        if timeout:
            conn = httplib.HTTPConnection(host, port, timeout=timeout)
        else:
            conn = httplib.HTTPConnection(host, port)
        return conn
    except Exception as e:
        printerror('ERROR: failed to open connection. ErrorValue: {}'.format(str(e)))
        exit(0)

def spec_request_headers(qheaders):
    '''
    parse/validate the request header specifications and return a normal dict()
    '''
    hd = {}
    if qheaders:
        for h in qheaders:
            try:
                h = str(h)
                c = str(qheaders[h])
                if c:
                    hd[h] = c
                    print('request header spec: \t{}  :  {}'.format(h, c))
            except:
                printerror('ERROR: invalid request header value {}  :  {}'.format(h, c))
    return hd

def spec_request_body(qparams, qfile):
    '''
    parse/validate the request body specifications and return:
    1. str if qparams is specified
    2. file object if qfile is specified
    3. use qparams if both are specified
    '''
    if qparams:
        pd = {}
        for p in qparams:
            try:
                p = str(p)
                v = str(qparams[p])
                if v:
                    pd[p] = v
                    print('request param spec: \t{} : {}'.format(p, v))
            except:
                printerror('ERROR: invalid request param pair {} : {}'.format(p, v))
        if pd:
            return urllib.urlencode(pd)

    # try qfile if it has come to this point
    if qfile:
        try:
            print('request file spec: \t{}'.format(qfile))
            return open(qfile, 'r')
        except Exception as e:
            printerror('ERROR: failed to open request body file {}. ErrorValue: {}'.format(
                qfile, str(e)))
    return ''


def check_equal(term, expected, actual):
    '''
    return True if expected == actual
    '''
    print('\n - {}'.format(term))
    print('EXPECT: {}'.format(expected))
    print('ACTUAL: {}'.format(actual))
    if expected == actual:
        printgreen('[OK]')
        return True
    else:
        printred('[FAIL]')
        return False

def verify_resp_status(resp, rstatus):
    try:
        rstatus = int(rstatus)
    except:
        printerror('ERROR: invalid resp_status value {}'.format(rstatus))
        return False
    return check_equal('STATUS', rstatus, resp.status)

def verify_resp_reason(resp, rreason):
    try:
        rreason = str(rreason)
    except:
        printerror('ERROR: invalid resp_reason value {}'.format(rreason))
        return False
    return check_equal('REASON', rreason, resp.reason)

def verify_resp_version(resp, rversion):
    try:
        rversion = str(rversion)
    except:
        printerror('ERROR: invalid resp_version value {}'.format(rversion))
        return False
    version = http_version_map.get(resp.version)
    if not version:
        version = resp.version
    return check_equal('VERSION', rversion, version)

def verify_resp_headers(resp, rheaders):
    ret = True
    try:
        for header in rheaders:
            header = str(header)
            content = str(rheaders[header])
            if content and not check_equal(
                'HEADER: ' + header, content, resp.getheader(header, None)):
                ret = False
        return ret
    except Exception as e:
        printerror('ERROR: failed to verify response headers. ErrorValue: {}'.format(str(e)))
        return False

def verify_resp_verifun(module, resp, rverifun):
    '''
    call module.rverifun() on the raw str data returned by resp.read(),
    this allows testers to apply more specific verifications on the response body
    '''
    try:
        rverifun = str(rverifun)
        print('\n - VERIFICATION by {}'.format(rverifun))
        if getattr(module, rverifun)(resp.read()):
            printgreen('[OK]')
            return True
        else:
            printred('[FAIL]')
            return False
    except Exception as e:
        printerror('ERROR: failed verifying function {}. ErrorValue: {}'.format(rverifun, str(e)))
        return False
            

def run_case(conn, module, case, id=None):
    '''
    run single case
    return True if case passed, False otherwise
    '''

    #
    # parse and validate case arguments
    #
    name = case.get('NAME')
    descr = case.get('DESCRIPTION')
    
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
        printhigh('case ID: \t\t{}'.format(id))
    if name:
        printhigh('case name: \t\t{}'.format(name))
    if descr:
        printhigh('({})'.format(descr))

    if not qpath:
        print('WARN: path is not specified, use default path: /')
        qpath = "/"
    print('\nrequest path: \t\t{}'.format(qpath))

    if not qmethod:
        print('WARN: method is not specified, use default method: GET')
        qmethod = "GET"
    if not qmethod in ["GET", "POST", "HEAD"]:
        printerror('ERROR: method {} not supported'.format(qmethod))
        printred('\ntest failed')
        return False
    print('request method: \t{}'.format(qmethod))

    headers = spec_request_headers(qheaders)
    body = spec_request_body(qparams, qfile)

    #
    # send request to host server and obtain response
    #
    ret = True
    try:
        conn.request(qmethod, qpath, body, headers)
    except Exception as e:
        printerror('ERROR: request failed. ErrorValue: {}'.format(str(e)))
        printred('\ntest failed')
        ret = False
    if body and type(body) is file:
        body.close()
    if not ret:
        return ret

    resp = conn.getresponse()

    #
    # verifications
    #
    print('\nstart verifications...')
    ret = True

    if rstatus and not verify_resp_status(resp, rstatus):
        ret = False

    if rreason and not verify_resp_reason(resp, rreason):
        ret = False

    if rversion and not verify_resp_version(resp, rversion):
        ret = False

    if rheaders and not verify_resp_headers(resp, rheaders):
        ret = False

    if rverifun and not verify_resp_verifun(module, resp, rverifun):
        ret = False

    # summary
    if not ret:
        printred('\ntest failed')
    else:
        printgreen('\ntest passed')
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
    print('===========================================')
    print('total:   {}'.format(nr_run))
    print('passed:  {}'.format(nr_success))
    print('failed:  {}'.format(nr_run - nr_success))


'''
===============================================================================
'''
def main(test_cases_file, lib, config, disable_color=False):
    if not test_cases_file:
        printerror('ERROR: must specify test cases file')
        exit(0)
    print('test cases file: \t{}'.format(test_cases_file))

    module = None
    if lib:
        try:
            module = __import__(lib)
        except Exception as e:
            printerror('ERROR: failed to import lib: {}. ErrorValue: {}'.format(lib, str(e)))
            exit(0)
        print('verification lib: \t{}'.format(lib))
    if config:
        print('configuration file: \t{}'.format(config))

    global COLOR
    COLOR = not disable_color

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
    parser.add_argument('--disable_color', help = 'disable printing to terminal with multiple colors', action = "store_true")

    args = vars(parser.parse_args())

    main(**args)