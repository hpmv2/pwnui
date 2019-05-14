import struct
import traceback
import service_pb2_grpc as rpc
import service_pb2 as pb
import grpc
from concurrent.futures import as_completed
from concurrent.futures import Future
from concurrent.futures import ThreadPoolExecutor
import logging
import time
import sys
logging.basicConfig()


class RemoteIOSession:
    def __init__(self, session_id):
        self.session_id = session_id
        self.read_future = Future()
        self.write_future = Future()
        self.done_future = Future()
        self.response_future = Future()
        self.executor = ThreadPoolExecutor(max_workers=1)

    def _request_streaming(self):
        req = pb.IOServerRequest()
        req.start_id = self.session_id
        yield req

        while True:
            for op in as_completed([self.read_future, self.write_future, self.done_future]):
                if op is self.read_future:
                    read = self.read_future.result()
                    print 'Reading with request', read
                    req = pb.IOServerRequest()
                    req.read.CopyFrom(read)
                    self.read_future = Future()
                    yield req
                elif op is self.write_future:
                    write = self.write_future.result()
                    print 'Writing with request', write
                    req = pb.IOServerRequest()
                    req.write.CopyFrom(write)
                    self.write_future = Future()
                    yield req
                else:
                    return
                break

    def _do_rpc(self):
        with grpc.insecure_channel('localhost:12345') as channel:
            print 'Created channel', channel
            stub = rpc.UIServiceStub(channel)
            print 'Created stub', stub

            response_stream = stub.IOServerConnect(self._request_streaming())

            for resp in response_stream:
                self.response_future.set_result(resp)

    def start(self):
        self.executor.submit(self._do_rpc)
        self._wait_for_response()

    def stop(self):
        self.done_future.set_result(None)

    def _wait_for_response(self):
        result = self.response_future.result()
        self.response_future = Future()
        return result

    def _do_read(self, req):
        self.read_future.set_result(req)
        return self._wait_for_response().read

    def _do_write(self, req):
        self.write_future.set_result(req)
        return self._wait_for_response()

    def read32(self):
        req = pb.IOReadRequest()
        ele = req.chunks.add()
        ele.little_endian_32 = True
        resp = self._do_read(req)
        return struct.unpack('<I', resp.data)[0]

    def readregex(self, regex, *groups):
        req = pb.IOReadRequest()
        ele = req.chunks.add()
        ele.regex.regex = regex
        ele.regex.groups[:] = groups
        resp = self._do_read(req).chunks[0]
        result = [resp.data]
        for (data_type, data) in zip(groups, resp.groups):
            result.append(self._parse_element(data, data_type))
        return tuple(result)

    def _parse_element(self, data, data_type):
        if data_type == pb.IOET_RAW:
            return data
        if data_type == pb.IOET_DECIMAL32:
            return int(data)
        if data_type == pb.IOET_HEX32:
            return struct.unpack('<I', data)[0]
        return data

    def write(self, data):
        req = pb.IOWriteRequest()
        ele = req.chunks.add()
        ele.data = data
        self._do_write(req)

    def writeint(self, val):
        req = pb.IOWriteRequest()
        ele = req.chunks.add()
        ele.decimal_integer = val
        self._do_write(req)

    def run_script(self, content):
        s = self
        try:
            exec(content)
        except Exception as e:
            traceback.print_exc()
            quit()


if __name__ == '__main__':
    print 'Running with ', sys.argv
    session_id = int(sys.argv[1])
    if session_id == -1:
        # Testing.
        with grpc.insecure_channel('localhost:12345') as channel:
            stub = rpc.UIServiceStub(channel)
            req = pb.NewSessionRequest()
            req.binary = '/bin/cat'
            resp = stub.NewSession(req)
            session_id = resp.id
            print('Created session', session_id)
    script = sys.argv[2]
    script_content = open(script).read()
    session = RemoteIOSession(session_id)
    session.start()
    session.run_script(script_content)
    session.stop()
