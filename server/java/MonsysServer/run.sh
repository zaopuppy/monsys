#!/bin/bash


# CLASSPATH="bin:libs/netty-all-4.0.19.Final.jar:libs/protobuf-java-2.5.0.jar"
CLASSPATH="bin:libs/monsysserver.jar:libs/mysql-connector-java-5.1.31-bin.jar:libs/netty-all-4.0.19.Final.jar:libs/protobuf-java-2.5.0.jar"
JAVA_OPTS="-Xms256m -Xmx2048m"

# check limitations
# launchctl limit
# change it
# launchctl limit maxfiles 400000 unlimited
# write to /etc/launchd.conf for permanent saving
# limit maxfiles 400000 unlimited

main()
{
    case "$1" in
        "server")
            if [ "$2" == "debug" ]; then
                JAVA_OPTS="${JAVA_OPTS} -Xdebug -Xrunjdwp:transport=dt_socket,server=y,address=5005"
            fi
            java -classpath "$CLASSPATH" $JAVA_OPTS com/letsmidi/monsys/push/PushServer
            ;;
        "client")
            if [ "$2" == "debug" ]; then
                JAVA_OPTS="${JAVA_OPTS} -Xdebug -Xrunjdwp:transport=dt_socket,server=y,address=5005"
            fi
            java -classpath "$CLASSPATH" $JAVA_OPTS com/letsmidi/monsys/push/PushClient
            ;;
        *)
            echo "Bad arguments"
            ;;
    esac
}

cd $(dirname $0)
main "$@"

