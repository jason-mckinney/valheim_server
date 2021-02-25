FROM centos:8

RUN yum update -y \
    && yum install glibc.i686 libstdc++.i686 gcc -y \
    && yum clean all

RUN mkdir /steamcmd \
    && cd steamcmd \
    && curl -sqL "https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz" | tar zxvf -

COPY playercount_poll.c /src/

RUN gcc -Werror -o /usr/bin/playercount_poll /src/playercount_poll.c \
    && yum remove gcc -y \
    && yum autoremove \
    && yum clean all

COPY server_start.sh /valheim/

VOLUME [ "/valheim/save" ]
WORKDIR /valheim

ENTRYPOINT [ "/bin/bash", "/valheim/server_start.sh" ] 
