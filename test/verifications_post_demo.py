from bs4 import BeautifulSoup
import json

def check_input(data):
    if not type(data) is str:
        raise Exception("Input argument is not of type str")

def verify_echo(data):
    '''
    Simply echos the data
    '''
    check_input(data)

    print('received raw data of length {}'.format(len(data)))
    print(data)
    return True

def verify_cjango_404(data):
    '''
    GET request on an undefined path
    '''
    check_input(data)

    print('received raw data of length {}'.format(len(data)))
    target = "Cjango: 404 Page Not Found"
    if data != target:
        print('contents mismatch')
        print('expect: {}'.format(target))
        print('actual: {}'.format(data))
        return False

    return True

def verify_get_home(data):
    '''
    GET request on page with static components
    '''
    check_input(data)

    print('received raw data of length {}'.format(len(data)))
    soup = BeautifulSoup(data, 'html.parser')

    target = "Cjango Demo"
    if soup.find('title').text != target:
        return False
    print('title OK: {}'.format(target))

    target = "Please log in"
    if soup.find('h2').text != target:
        return False
    print('contents OK')

    return True
