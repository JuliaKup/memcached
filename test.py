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
    return res

if __name__ == "__main__":
	port = sys.argv[1]
	mc = subprocess.Popen(['./server', port])

	mcg = "add abc 1 0 1\r\n5\r\n"
	exp_res = "STORED\r\n"
	result = connect(port, mcg)
	assert result == exp_res, "expected: '%s' got '%s'" % (exp_res, result)

	mcg = "get abc\r\n"
	exp_res = "VALUE abc 1 1\r\n5\r\nEND\r\n"
	result = connect(port, mcg)
	assert result == exp_res, "expected: '%s' got '%s'" % (exp_res, result)

	mcg = "set abc 1 0 2\r\n55\r\n"
	exp_res = "STORED\r\n"
	result = connect(port, mcg)
	assert result == exp_res, "expected: '%s' got '%s'" % (exp_res, result)

	mcg = "get abc\r\n"
	exp_res = "VALUE abc 1 2\r\n55\r\nEND\r\n"
	result = connect(port, mcg)
	assert result == exp_res, "expected: '%s' got '%s'" % (exp_res, result)

	mcg = "delete abc\r\n"
	exp_res = "DELETED\r\n"
	result = connect(port, mcg)
	assert result == exp_res, "expected: '%s' got '%s'" % (exp_res, result)
	print("OK")
	mc.kill