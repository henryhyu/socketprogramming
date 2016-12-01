# import socket module and sys module
import socket
import sys

# majority of basic setup code from python wiki

# create socket object [IPv4, TCP]
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# connect to a server
# need ip address of a server, need to know hostname of a server
ip = sys.argv[1]

# now need port number
port = int(sys.argv[2])

# address of server we are going to connect to
address = (ip,port)
client.connect(address)

# now we have an established connection with the server
string = raw_input("Enter string: ")

# client sends user input to the server
client.send(string)

# print response from server
counter = 0

while True:
    # receive number from server
    number = client.recv(128)
    if (number == "Sorry, cannot compute!"):
        print "From server: ", number
        break
    # split the delimters into an array
    printnumber = number.split(" ")
    for x in range (0, len(printnumber)):
        # print all the numbers that aren't the delimiter
        if(printnumber[x] != ''):
            print "From server: ", printnumber[x]
        if(len(printnumber[x]) == 1 or not printnumber[x].isdigit()):
            break
    # done streaming
    if (not printnumber[0].isdigit()):
        break

# close the connection
client.close()
