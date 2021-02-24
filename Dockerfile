FROM centos:8

RUN yum update -y \
    && yum install glibc.i686 libstdc++.i686 -y \
    && yum clean all

RUN mkdir /steamcmd \
    && cd steamcmd \
    && curl -sqL "https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz" | tar zxvf -

COPY server_start.sh /valheim/

VOLUME [ "/valheim/save" ]
WORKDIR /valheim

ENTRYPOINT [ "/bin/bash", "/valheim/server_start.sh" ] 
