all: 
	g++ -o client client.cpp 
	g++ -o awsoutput aws.cpp 
	g++ -o serverAoutput serverA.cpp 
	g++ -o serverBoutput serverB.cpp 
	g++ -o serverCoutput serverC.cpp
	g++ -o monitoroutput monitor.cpp

.PHONY: serverA	
serverA:
	./serverAoutput

.PHONY: serverB
serverB:
	./serverBoutput

.PHONY: serverC	
serverC:
	./serverCoutput

.PHONY: aws
aws:
	./awsoutput

.PHONY: monitor	
monitor:
	./monitoroutput



