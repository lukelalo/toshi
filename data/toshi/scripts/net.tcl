# ---------------------------------------------------------------------------
#   net.tcl
#   =======
#
#   Creates a enet server and provides methods for listening and connecting
# ---------------------------------------------------------------------------

new nenetserver /sys/servers/enet
new nroot /game/nets
/game.setnet /game/nets

proc servidorRed { port } {
    /sys/servers/enet.createhost toshiServer
    /sys/share/network/hosts/toshiServer.listen ANY $port 1 0 0
    
    new nnetserver /game/nets/server
    /game/nets/server.sethost /sys/share/network/hosts/toshiServer
}

proc clienteRed { address port } { 
    /sys/servers/enet.createhost toshiClient
    set peer [/sys/share/network/hosts/toshiClient.connect $address $port 1 0 0]

    new nnetclient /game/nets/client
    /game/nets/client.setpeer $peer
    /game/nets/client.setchannel 0
}

proc desconectarCliente { } {
    #/sys/share/network/hosts/toshiClient.disconnect

}

proc desconectarServidor { } {
    #/sys/share/network/hosts/toshiServer.disconnect
}

#if { $::EsServidor } {
#	servidorRed 5050
#} else {
#	clienteRed localhost 5050
#}

