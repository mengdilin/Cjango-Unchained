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

def verify_cjango_homepage(data):
    '''
    GET request on page with static components
    '''
    check_input(data)

    print('received raw data of length {}'.format(len(data)))
    soup = BeautifulSoup(data, 'html.parser')

    target = "Cjango - C++ Web App Framework"
    if soup.find('title').text != target:
        return False
    print('title OK: {}'.format(target))

    if soup.find('style').get('type') != 'text/css':
        return False
    print('css OK')

    target = "Welcome to Cjango Unchained!"
    if soup.find('h1').text != target:
        return False
    print('contents OK')

    return True

def verify_cjango_official_site(data):
    '''
    GET request on page with static components
    '''
    check_input(data)

    print('received raw data of length {}'.format(len(data)))
    soup = BeautifulSoup(data, 'html.parser')

    target = "Cjango - C++ Web App Framework"
    if soup.find('title').text != target:
        return False
    print('title OK: {}'.format(target))

    if soup.find('style').get('type') != 'text/css':
        return False
    print('css OK')

    target = "static/js/hello.js"
    if soup.find('script').get('src') != target:
        return False
    print('js OK')

    return True

def verify_cjango_json(data):
    '''
    GET request on json data
    '''
    check_input(data)

    print('received raw data of length {}'.format(len(data)))

    jd = json.loads(data)
    if not 'parents' in jd:
        return False
    print('json OK')

    return True