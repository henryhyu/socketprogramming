# import socket module and sys module
import socket
import sys

# majority of basic setup code from python wiki

# create socket object [IPv4, UDP]
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# need to bind object to an address

# port number from user input
port = int(sys.argv[1])

# address for our server to bind to
address = ('',port)
server.bind(address)

# receive data
data, addr = server.recvfrom(128)

# perform functions
while True:
     # make sure data is a string
    assert type(data) is str

    # break if length is 1 and is a digit
    if len(data) == 1 and data.isdigit():
        break
    
    if data.isdigit():
        sum = 0
        for character in data:
            sum += int(character)
            
        # send data back to client
        server.sendto(str(sum), addr)

        # assign new sum to data
        data = str(sum)
    else:
        server.sendto("Sorry, cannot compute!", addr)
        break

