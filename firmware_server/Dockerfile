FROM php:7.3-apache

COPY ./app /var/www/html
COPY ./config.cpp /config.cpp
COPY ./docker-php-entrypoint /usr/local/bin/docker-php-entrypoint

RUN chmod 755 /usr/local/bin/docker-php-entrypoint