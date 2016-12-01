# import socket module and sys module
import socket
import sys

# majority of basic setup code from python wiki

# create socket object [IPv4, UDP]
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# connect to a server
# need ip address of a server, need to know hostname of a server
ip = sys.argv[1]

# now need port number
port = int(sys.argv[2])

# address of server we are going to connect to
address = (ip,port)

string = raw_input("Enter string: ")
client.sendto(string, address)

# print response from server
while True:
    number = client.recv(128)
    print "From server: ", number
    
    # if length of number is 1
    # or the first character of the number is not a digit,
    # break
    if(len(number) == 1 or not number[0].isdigit()):
        break

