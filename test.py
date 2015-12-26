import socket
import sys
import subprocess
from multiprocessing import Pool
import time

port = 54322

def connect(port, msg):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("localhost", int(port)))

    s.send(msg)
    
    res = s.recv(4096)
    print(res)
    return res

if __name__ == "__main__":
	port = sys.argv[1]
	mc = subprocess.Popen(['./server', port])
	time.sleep(1)
	mcg = "add abc 1 0 1\r\n5\r\n"
	exp_res = "STORED"
	pool = Pool(processes = 10)              # start 100 worker processes
	result = pool.apply_async(connect, [port, mcg], callback=lambda x: x == exp_res)    # evaluate asynchronously
	result.wait()

	mcg = "get abc\r\n"
	exp_res = "VALUE abc 1 1\r\n5\r\nEND"
	pool = Pool(processes = 10)              # start 100 worker processes
	result = pool.apply_async(connect, [port, mcg], callback=lambda x: x == exp_res)    # evaluate asynchronously
	result.wait()

	mcg = "set abc 1 0 2\r\n55\r\n"
	exp_res = "STORED"
	pool = Pool(processes = 10)              # start 100 worker processes
	result = pool.apply_async(connect, [port, mcg], callback=lambda x: x == exp_res)    # evaluate asynchronously
	result.wait()

	mcg = "get abc\r\n"
	exp_res = "VALUE abc 1 2\r\n55\r\nEND"
	pool = Pool(processes = 10)              # start 100 worker processes
	result = pool.apply_async(connect, [port, mcg], callback=lambda x: x == exp_res)    # evaluate asynchronously
	result.wait()

	mcg = "delete abc\r\n"
	exp_res = "DELETED"
	pool = Pool(processes = 10)              # start 100 worker processes
	result = pool.apply_async(connect, [port, mcg], callback=lambda x: x == exp_res)    # evaluate asynchronously
	result.wait()

	mc.kill