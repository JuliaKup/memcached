import socket
import sys
import subprocess

port = 54322

def check_echo(msg, exp_res):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("localhost", int(port)))

    s.send(msg)
    
    res = s.recv(len(msg))
    assert exp_res == res

if __name__ == "__main__":
	port = sys.argv[1]
	mc = subprocess.Popen(['./server', port])

	mcg = "get abc"
	exp_res = "Forget your troubles 1 8\r\n Be happy\r\n"
	check_echo(mcg, exp_res)

	mcg = "set abc 1 1445128601 5"
	exp_res = "SORTED"
	check_echo(mcg, exp_res)

	mcg = "add abc 1 1445128601 5"
	exp_res = "SORTED"
	check_echo(mcg, exp_res)

	mcg = "delete abc"
	exp_res = "SORTED"
	check_echo(mcg, exp_res)

	mc.kill()