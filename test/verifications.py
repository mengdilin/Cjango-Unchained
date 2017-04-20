def check_input(data):
    if not type(data) is str:
        raise Exception("Input argument is not of type str")

def verify_case_1(data):
    check_input(data)

    print('received raw data of length {}'.format(len(data)))
    return True

def verify_case_2(data):
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