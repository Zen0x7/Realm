FROM iantorres/boosted:latest

COPY src .

RUN cmake . && \
    make

EXPOSE 8000

COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

CMD ["/usr/bin/supervisord"]