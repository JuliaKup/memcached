import socket
import sys
import subprocess

port = 54322

def check_echo(msg, exp_res):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("localhost", int(port)))

    s.send(msg)
    
    res = s.recv(len(exp_res))
    print(res)
    assert exp_res == res

if __name__ == "__main__":
	port = sys.argv[1]
	mc = subprocess.Popen(['./server', port])

	#mcg = "get abc\r\n"
	#exp_res = "NOT_FOUND"
	#check_echo(mcg, exp_res)

	mcg = "set abc 1 1\r\n5\r\n"
	exp_res = "STORED"
	check_echo(mcg, exp_res)
	print(1)

	mcg = "get abc\r\n"
	exp_res = "VALUE abc 0 1\r\n5\r\n"
	check_echo(mcg, exp_res)
	print(3)

	mcg = "delete abc\r\n"
	exp_res = "DELETED"
	check_echo(mcg, exp_res)

	mc.kill()