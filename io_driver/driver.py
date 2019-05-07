import service_pb2_grpc as rpc
import service_pb2 as pb
import grpc
from concurrent.futures import as_completed
from concurrent.futures import Future
from concurrent.futures import ThreadPoolExecutor
import logging
logging.basicConfig()

session_id = None

# proof-of-concept code quality
read_future = Future()
write_future = Future()
done_future = Future()
response_future = Future()


def request_streaming():
    global read_future
    global write_future
    req = pb.IOServerRequest()
    req.start_id = session_id
    yield req

    while True:
        for op in as_completed([read_future, write_future, done_future]):
            if op is read_future:
                read = read_future.result()
                print 'Reading with request', read
                req = pb.IOServerRequest()
                req.read.CopyFrom(read)
                read_future = Future()
                yield req
            elif op is write_future:
                write = write_future.result()
                print 'Writing with request', write
                req = pb.IOServerRequest()
                req.write.CopyFrom(write)
                write_future = Future()
                yield req
            else:
                return
            break


def do_rpc():
    global response_future
    global session_id
    with grpc.insecure_channel('localhost:12345') as channel:
        print('Created channel', channel)
        stub = rpc.UIServiceStub(channel)
        print('Created stub', stub)

        # just for demo purposes - create a session to use. In reality the UI will be doing that.
        req = pb.NewSessionRequest()
        req.binary = '/bin/cat'
        resp = stub.NewSession(req)
        session_id = resp.id
        print('Created session', session_id)

        response_stream = stub.IOServerConnect(request_streaming())

        for resp in response_stream:
            response_future.set_result(resp)


executor = ThreadPoolExecutor(max_workers=1)
executor.submit(do_rpc)


def wait_for_response():
    global response_future
    result = response_future.result()
    response_future = Future()
    return result


def remote_read32():
    req = pb.IOReadRequest()
    ele = req.elements.add()
    ele.little_endian_32 = True
    read_future.set_result(req)
    return wait_for_response()

def remote_readregex(regex, *groups):
    req = pb.IOReadRequest()
    ele = req.elements.add()
    ele.regex.regex = regex
    ele.regex.groups[:] = groups
    read_future.set_result(req)
    return wait_for_response()

def remote_write(data):
    req = pb.IOWriteRequest()
    ele = req.elements.add()
    ele.data = data
    write_future.set_result(req)
    wait_for_response()

def remote_writeint(val):
    req = pb.IOWriteRequest()
    ele = req.elements.add()
    ele.decimal_integer = val
    write_future.set_result(req)
    wait_for_response()

# Just proof-of-concept for now to demonstrate the implementability of read/write functions which
# go through our server to perform the actual I/O.
wait_for_response()
remote_write('ABCD I have 100 dollars...!')
print remote_read32()
print remote_readregex('have (\\d+) dollars', pb.IOET_DECIMAL32)
done_future.set_result(1)
