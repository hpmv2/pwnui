from io_ops import *
import sys
import time
import logging
from concurrent.futures import ThreadPoolExecutor
from concurrent.futures import Future
from concurrent.futures import as_completed
import grpc
import service_pb2 as pb
import service_pb2_grpc as rpc
import traceback
import struct

logging.basicConfig()

# sys.stderr.write("LOL")


class RemoteIOSession:
    def __init__(self, session_id):
        self.session_id = session_id
        self.req_future = Future()
        self.done_future = Future()
        self.response_future = Future()
        self.executor = ThreadPoolExecutor(max_workers=1)
        self.manager = IoManager(self._read, self._write, self._sync)

    def _request_streaming(self):
        req = pb.IOServerRequest()
        req.start_id = self.session_id
        yield req

        while True:
            for op in as_completed([self.req_future, self.done_future]):
                if op is self.req_future:
                    req = self.req_future.result()
                    self.req_future = Future()
                    yield req
                else:
                    return
                break

    def _do_rpc(self):
        try:
            with grpc.insecure_channel('localhost:12345') as channel:
                stub = rpc.UIServiceStub(channel)

                response_stream = stub.IOServerConnect(
                    self._request_streaming())

                for resp in response_stream:
                    self.response_future.set_result(resp)
        except:
            traceback.print_exc(file=sys.stderr)

    def start(self):
        self.executor.submit(self._do_rpc)
        self._wait_for_response()

    def stop(self):
        self.done_future.set_result(None)

    def _wait_for_response(self):
        result = self.response_future.result()
        self.response_future = Future()
        return result

    def _read(self, chain_id, read):
        req = pb.IOServerRequest()
        req.read.CopyFrom(read)
        req.read.chain_id = chain_id
        self.req_future.set_result(req)
        return self._wait_for_response().ack

    def _write(self, write):
        req = pb.IOServerRequest()
        req.write.CopyFrom(write)
        self.req_future.set_result(req)
        return self._wait_for_response().ack

    def _sync(self, sync):
        req = pb.IOServerRequest()
        req.sync.CopyFrom(sync)
        self.req_future.set_result(req)
        return self._wait_for_response().result

    def __rshift__(self, op):
        return self.manager >> op

    def __lshift__(self, op):
        return self.manager << op

    def run_script(self, content):
        s = self
        try:
            exec(content)
        except Exception as e:
            print(traceback.format_exc())
            quit()


if __name__ == '__main__':
    session_id = int(sys.argv[1])
    if session_id == -1:
        # Testing.
        with grpc.insecure_channel('localhost:12345') as channel:
            stub = rpc.UIServiceStub(channel)
            req = pb.NewSessionRequest()
            req.binary = '/bin/cat'
            resp = stub.NewSession(req)
            session_id = resp.id
    script = sys.argv[2]
    script_content = open(script).read()
    session = RemoteIOSession(session_id)
    session.start()
    session.run_script(script_content)
    session.stop()
