import service_pb2_grpc as rpc
import service_pb2 as pb
import grpc
from concurrent.futures import as_completed
from concurrent.futures import Future
from concurrent.futures import ThreadPoolExecutor

session_id = None

# proof-of-concept code quality
readn_future = Future()
write_future = Future()
done_future = Future()
response_future = Future()


def request_streaming():
    global readn_future
    global write_future
    req = pb.IOServerRequest()
    req.start_id = session_id
    yield req

    while True:
        for op in as_completed([readn_future, write_future, done_future]):
            if op is readn_future:
                readn = readn_future.result()
                req = pb.IOServerRequest()
                req.readn = readn
                readn_future = Future()
                yield req
            elif op is write_future:
                write = write_future.result()
                req = pb.IOServerRequest()
                req.write = write
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


def remote_readn(n):
    readn_future.set_result(n)
    return wait_for_response().content

def remote_write(data):
    write_future.set_result(data)
    wait_for_response()

# Just proof-of-concept for now to demonstrate the implementability of read/write functions which
# go through our server to perform the actual I/O.
wait_for_response()
remote_write('ABCDEFGH')
print remote_readn(8)
remote_write('IJKLMNO')
print remote_readn(4)
print remote_readn(2)
print remote_readn(1)
done_future.set_result(1)
