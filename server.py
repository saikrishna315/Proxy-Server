#import socket module

from socket import *

Portno=12001

ssoc = socket(AF_INET, SOCK_STREAM)

ssoc.bind(('',Portno))

ssoc.listen(1)

print 'the web server is up on port:',Portno


while True:



    print 'Ready to serve...'

    conSoc, addr = ssoc.accept()



    try:

        msg = conSoc.recv(1024)

        print msg, '::',msg.split()[0],':',msg.split()[1]

        files = msg.split()[1]

        print files,'||',files[1:]

        flen = open(files[1:])

        outputvalue = flen.read()
        
        print outputvalue
        

        conSoc.send('\n HTTP/1.1 200 OK \n\n')

        conSoc.send(outputvalue)

        for i in range(0, len(outputvalue)):

            conSoc.send(outputvalue[i])

        conSoc.close()

    
    except IOError:

	conSoc.send('\n HTTP/1.1 404 Not Found \n\n')

        
        conSoc.close()

        #ssoc.close()
			
