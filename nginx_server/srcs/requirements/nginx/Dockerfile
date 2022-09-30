FROM debian:buster
ARG DEBIAN_FRONTEND=noninteractive
RUN printf '#! /bin/sh\nexit 0' > /usr/sbin/policy-rc.d; \
	echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections; \
	apt-get update && \
	apt-get install --no-install-recommends --no-install-suggests -y nginx && \
	echo "daemon off;" >> /etc/nginx/nginx.conf
COPY ./default /etc/nginx/sites-available/default
RUN chmod 0744 /etc/nginx/sites-available/default; \
	mkdir -p /var/www/html/basic; \
	mkdir -p /var/www/html/default;
COPY ./indexbasic.html /var/www/html/basic/index.html
COPY ./indexdefault.html /var/www/html/default/index.html
CMD ["nginx"]
