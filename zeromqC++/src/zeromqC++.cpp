
#include <iostream>
#include <zmq.hpp>
#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
	zmq::context_t context (1);
	zmq::socket_t publisher(context, ZMQ_PUB);
	publisher.bind("tcp://*:5556");

	std::cout << "starting server\n";

	while(1){
		int number = random() % 100;

		zmq::message_t message(20);

		snprintf ((char *) message.data(), 20, "displacement %d", number);
		publisher.send(message);
	}
}
