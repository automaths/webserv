inception:	
		cd srcs && docker-compose up --remove-orphans -d

all:		inception

clean:	
		cd srcs && docker-compose down


fclean: 	clean
		docker rm -f nginx; \
		docker rmi -f nginx:inception; \
		true;

re:		clean inception

.PHONY: inception all bonus clean fclean re
