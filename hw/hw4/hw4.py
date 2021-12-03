#!/usr/bin/env python3

from concurrent import futures
import sys  # For sys.argv, sys.exit()
import socket  # for gethostbyname()
import time
import grpc
import traceback
import csci4220_hw4_pb2
import csci4220_hw4_pb2_grpc
import heapq

_ONE_DAY_IN_SECONDS = 60 * 60 * 24

global k_buckets
global id_set # all of port id in k_buckets
global stored
global local_id
global my_port
global my_address
global path #list of nodes that we already inquiried

def update_id_set():
    global k_buckets
    global id_set # all of port id in k_buckets
    global stored
    global local_id
    global my_port
    global my_address
    global path #list of nodes that we already inquiried
    id_set.clear()
    for nodes in k_buckets.values():
        if len(nodes) != 0:
            for node in nodes:
                id_set.append(node[0])

def update_buckets(node_list):
    global k_buckets
    try:
        for new_node in node_list:
            # case when the new_node should in 3s bucket
            if new_node[0] ^ local_id >= 8:
                if len(k_buckets[3]) == 2 and new_node[0] != k_buckets[3][1][0]:
                    k_buckets[3][0] = k_buckets[3][1]
                    k_buckets[3][1] = new_node
                if len(k_buckets[3]) == 1 and new_node[0] != k_buckets[3][0][0]:
                    k_buckets[3].append(new_node)
                if len(k_buckets[3]) == 0:
                    k_buckets[3].append(new_node)
            # case when the new_node should in 2s bucket
            elif new_node[0] ^ local_id >= 4:
                if len(k_buckets[2]) == 2 and new_node[0] != k_buckets[2][1][0]:
                    k_buckets[2][0] = k_buckets[2][1]
                    k_buckets[2][1] = new_node
                if len(k_buckets[2]) == 1 and new_node[0] != k_buckets[2][0][0]:
                    k_buckets[2].append(new_node)
                if len(k_buckets[2]) == 0:
                    k_buckets[2].append(new_node)
            # case when the new_node should in 1s bucket
            elif new_node[0] ^ local_id >= 2:
                if len(k_buckets[1]) == 2 and new_node[0] != k_buckets[1][1][0]:
                    k_buckets[1][0] = k_buckets[1][1]
                    k_buckets[1][1] = new_node
                if len(k_buckets[1]) == 1 and new_node[0] != k_buckets[1][0][0]:
                    k_buckets[1].append(new_node)
                if len(k_buckets[1]) == 0:
                    k_buckets[1].append(new_node)
            # case when the new_node should in 0s bucket
            elif new_node[0] ^ local_id > 0:
                if len(k_buckets[0]) == 1:
                    k_buckets[0][0] = new_node
                if len(k_buckets[0]) == 0:
                    k_buckets[0].append(new_node)
    except:
        traceback.print_exc()

def find_nearest(element_dictionary, destination):
    compare_number_list = []
    compare_number_element = []
    return_list = []
    min_number_index_list = []
    order = int()
    for i in element_dictionary:
        order = 0
        for j in element_dictionary[i]:
            compare_number_list.append(element_dictionary[i][order][0] ^ destination)
            compare_number_element.append(element_dictionary[i][order])
            order += 1

    min_number_index_list = map(compare_number_list.index, heapq.nsmallest(2, compare_number_list))
    for i in min_number_index_list:
        return_list.append(compare_number_element[i])
    return return_list

def client_find_node(target_nodeid):
    global id_set # all of port id in k_buckets
    global stored
    global local_id
    global my_port
    global my_address
    global path
    global k_buckets
    intersect_list = []
    found = False
    # judge whether we have already gone the port.
    if all(node in path for node in id_set):
        print('Could not find destination id %s' % target_nodeid)
        return
    # first need to judge whether <Nodeid> already in bucket
    for node in id_set:
        if target_nodeid == node:
            found = True
            print('Found destination id %s' % target_nodeid)
            return
    if found == False:
        nearest_list = find_nearest(k_buckets, target_nodeid)
        # before we begin,  we need to filter the nearest list by path
        intersect_list = [a_node for a_node in nearest_list if a_node not in path]
        if len(intersect_list) == 0:
            print('Could not find destination id %s' % target_nodeid)
            return
        # query for each nearest node
        for per_node in intersect_list:
            with grpc.insecure_channel(per_node[2] + ':' + str(per_node[1])) as channel:
                stub = csci4220_hw4_pb2_grpc.KadImplStub(channel)
                the_node = csci4220_hw4_pb2.Node(id = local_id, port = my_port, address = my_address)
                response = stub.FindNode(csci4220_hw4_pb2.IDKey(node = the_node,
                                        idkey = target_nodeid))
            # deal with nearest list from channel
            update_buckets([[response.responding_node.id, response.responding_node.port,
                                response.responding_node.address]])
            # update the path that already visited
            path.append(per_node[0])
            # update the buckets with nodes received
            for single_node in response.nodes:
                update_buckets([[single_node.id, single_node.port, single_node.address], ])

            # update idset
            update_id_set()
        client_find_node(target_nodeid)

def client_boot_strap(node_info):
    global id_set # all of port id in k_buckets
    global stored
    global local_id
    global my_port
    global my_address
    global path
    global k_buckets
    # use host name for building channel
    remote_addr = socket.gethostbyname(node_info[1])
    remote_port = int(node_info[2])

    with grpc.insecure_channel(remote_addr + ':' + str(remote_port)) as channel:
        stub = csci4220_hw4_pb2_grpc.KadImplStub(channel)
        the_node = csci4220_hw4_pb2.Node(id = local_id, port = my_port, address = my_address)
        response = stub.FindNode(csci4220_hw4_pb2.IDKey(node = the_node, idkey = local_id))
    update_buckets([[response.responding_node.id, response.responding_node.port,
                        response.responding_node.address],])
    # update buckets based on nodes responded
    for single_node in response.nodes:
        update_buckets([[single_node.id, single_node.port, single_node.address],])
    # update id set, for convenience,  we cratch all of id from buckets
    update_id_set()
    return response.responding_node.id


def client_find_value(target_key):
    global id_set # all of id in k_buckets
    global stored
    global local_id
    global my_port
    global my_address
    global path
    global k_buckets
    intersect_list = []
    found = False
    # judge whether we have already gone the port.
    if all(node in path for node in id_set):
        print('Could not find key %s' % target_key)
        return
    # first need to judge whether <Nodeid> already in bucket

    if found == False:
        if(len(stored) != 0):
            for i in stored:
                if i[0] == target_key:
                    print('Found data "%s" for key %s' % (i[1], target_key))
                    return

        nearest_list = find_nearest(k_buckets, target_key)
        intersect_list = [a_node for a_node in nearest_list if a_node not in path]
        if len(intersect_list) == 0:
            print('Could not find key %s' % target_key)
            return

        for per_node in intersect_list:
            with grpc.insecure_channel(per_node[2] + ':' + str(per_node[1])) as channel:
                stub = csci4220_hw4_pb2_grpc.KadImplStub(channel)
                the_node = csci4220_hw4_pb2.Node(id = local_id, port = my_port, address = my_address)
                response = stub.FindValue(csci4220_hw4_pb2.IDKey(node = the_node,
                                        idkey = target_key))
            # deal with nearest list from channel

            update_buckets([[response.responding_node.id, response.responding_node.port,
                                response.responding_node.address]])
            if response.mode_kv == 1:
                print('Found value "%s" for key %s' % (response.kv.value, target_key))
                return

            path.append(per_node[0])

            for single_node in response.nodes:
                # return_list.append([single_node.id, single_node.port, single_node.address])
                update_buckets([[single_node.id, single_node.port, single_node.address],])
                # id_set.add(single_node.id)
            # update buckets
            update_id_set()
            # update_buckets(return_list)
        client_find_value(target_key)

def store_helper(key_store,value_store):
    global id_set # all of port id in k_buckets
    global stored
    global local_id
    global my_port
    global my_address
    global path
    global k_buckets
    return_store = []
    return_store = client_store(key_store,value_store)

    if return_store == -1:
        print('Storing key %s at node %s' % (key_store,local_id))
        with grpc.insecure_channel(my_address + ':' + str(my_port)) as channel:
            stub = csci4220_hw4_pb2_grpc.KadImplStub(channel)
            the_node = csci4220_hw4_pb2.Node(id = local_id, port = my_port, address = my_address)
            response = stub.Store(csci4220_hw4_pb2.KeyValue(node = the_node,
                                key = key_store, value = value_store))

    if return_store != -1:

        print('Storing key %s at node %s' % (key_store, return_store[0][0]))
        with grpc.insecure_channel(return_store[0][2] + ':' + str(return_store[0][1])) as channel:
            stub = csci4220_hw4_pb2_grpc.KadImplStub(channel)
            the_node = csci4220_hw4_pb2.Node(id = local_id, port = my_port, address = my_address)

            response = stub.Store(csci4220_hw4_pb2.KeyValue(node = the_node,
                                key = key_store, value = value_store))

def client_store(keystore, valuestore):
    global id_set # all of port id in k_buckets
    global stored
    global local_id
    global my_port
    global my_address
    global path
    global k_buckets
    value_list = {}.fromkeys((0,1,2,3,4))
    value_list[4] = local_id

    return_distance = []
    return_distance = find_nearest(k_buckets,keystore)

    if (local_id ^ keystore) < (return_distance[0][0] ^ keystore):
        return -1
    elif return_distance[0][0] != local_id:
        return return_distance

# helper function for printing k_buckets
def print_k_buckets():
    global id_set # all of port id in k_buckets
    global stored
    global local_id
    global my_port
    global my_address
    global path
    global k_buckets
    for per_layer, nodes in k_buckets.items():
        print('%s:' % per_layer, end = '')
        for per_node in nodes:
            print(' %s:%s' % (per_node[0], per_node[1]), end = '')
        print()



def client_quit(local_id):
    global id_set # all of port id in k_buckets
    global stored
    global my_port
    global my_address
    global path
    global k_buckets

    for per_id in id_set:
        if per_id != local_id:
            print("Letting %d know I'm quitting." % per_id)

    for i in k_buckets:
        order = 0
        for j in k_buckets[i]:
            if (len(k_buckets[i][order]) != 0):
                with grpc.insecure_channel(k_buckets[i][order][2] + ':' + str(k_buckets[i][order][1])) as channel:
                    stub = csci4220_hw4_pb2_grpc.KadImplStub(channel)
                    the_node = csci4220_hw4_pb2.Node(id = local_id, port = my_port, address = my_address)
                    response = stub.Quit(csci4220_hw4_pb2.IDKey(node = the_node, idkey = 10))
            order += 1

    print("Shut down node %s" % local_id)
    exit(0)
class Server(csci4220_hw4_pb2_grpc.KadImplServicer):
    # handle findnode request
    def FindNode(self, request, context):
        global id_set # all of port id in k_buckets
        global stored
        global local_id
        global my_port
        global my_address
        global path
        global k_buckets
        # before we proceed, first we need to update buckets.
        update_buckets([[request.node.id, request.node.port, request.node.address],])
        # update id set
        update_id_set()

        print('Serving FindNode(%s) request for %s' % (request.idkey, request.node.id))
        # to find two nearest value
        return_list = []
        nodelist = csci4220_hw4_pb2.NodeList()
        nodelist.responding_node.id = local_id
        nodelist.responding_node.port = my_port
        nodelist.responding_node.address = my_address
        # two cases: if the request is from bootstrap, return the whole buckets
        # else we return the k nearest values in buckets
        if request.idkey == request.node.id:
            for nodes in k_buckets.values():
                if len(nodes) != 0:
                    for node in nodes:
                        return_list.append(node)
        else:
            return_list = find_nearest(k_buckets, request.idkey)

        for node in return_list:
            nodelist.nodes.add(id = node[0], port = node[1], address = node[2])
        return nodelist

    def Store(self, request, context):
        global id_set # all of port id in k_buckets
        global stored
        global local_id
        global my_port
        global my_address
        global path
        global k_buckets

        if request.node.id != local_id:
            print('Storing key %s value "%s"' % (request.key, request.value))

        idkey = csci4220_hw4_pb2.IDKey()

        if len(stored) == 0:
            stored.append([request.key,request.value])

        for i in stored:
            if(request.key != i[0]):
                stored.append([request.key,request.value])
                return idkey
            elif(request.key == i[0]):
                if(request.value == i[1]):
                    return idkey
                if(request.value != i[1]):
                    i[1] = request.value
                    return idkey

        print(stored)

    def FindValue(self, request, context):
        global id_set # all of port id in k_buckets
        global stored
        global local_id
        global my_port
        global my_address
        global path
        global k_buckets
        update_buckets([[request.node.id, request.node.port, request.node.address],])
        if(request.node.id != local_id):
            print('Serving FindKey(%s) request for %s' % (request.idkey, request.node.id))
        return_list = find_nearest(k_buckets, request.idkey)

        Wrapper = csci4220_hw4_pb2.KV_Node_Wrapper()
        Wrapper.responding_node.id = local_id
        Wrapper.responding_node.port = my_port
        Wrapper.responding_node.address = my_address

        o = 0
        p = []
        for i in stored:
            if i[0] == request.idkey:
                o = 1
                p = i

        if o==1:
            Wrapper.mode_kv = 1
        elif o==0:
            Wrapper.mode_kv = 0

        Wrapper.kv.node.id = local_id

        Wrapper.kv.node.port = my_port

        Wrapper.kv.node.address = my_address

        if p != []:
            Wrapper.kv.key = p[0]
            Wrapper.kv.value = p[1]

        i = 0
        for node in return_list:
            Wrapper.nodes.add(id = node[0], port = node[1], address = node[2])
        return Wrapper

    def Quit(self, request, context):
        global id_set # all of port id in k_buckets
        global stored
        global local_id
        global my_port
        global my_address
        global path
        global k_buckets

        k = 0
        for i in k_buckets:
            order = 0
            for j in k_buckets[i]:
                if (len(k_buckets[i][order]) != 0):
                    if k_buckets[i][order][0] == request.node.id:
                        print("Evicting quitting node %s from bucket %s" % (request.node.id, i))
                        k_buckets[i].remove(k_buckets[i][order])
                        update_id_set()
                        k = 1
                order += 1

        if k == 0:
            print("No record of quitting node %s in k-buckets." % request.node.id)

        quit_idkey = csci4220_hw4_pb2.IDKey()
        return quit_idkey

# run the client
def run():
    global id_set # all of port id in k_buckets
    global stored
    global local_id
    global my_port
    global my_address
    global path
    global k_buckets

    if len(sys.argv) != 4:
        print("Error, correct usage is {} [my id] [my port] [k]".format(sys.argv[0]))
        sys.exit(-1)

    local_id = int(sys.argv[1])
    my_port = int(sys.argv[2]) # add_insecure_port() will want a string
    k = int(sys.argv[3])
    my_hostname = socket.gethostname() # Gets my host name
    my_address = socket.gethostbyname(my_hostname) # Gets my IP address from my hostname

if __name__ == '__main__':
    run()
    global id_set # all of port id in k_buckets
    global stored
    global local_id
    global my_port
    global my_address
    global path
    global k_buckets
    k_buckets = {0:[], 1:[], 2:[], 3:[]}
    id_set = []
    stored = []
    path = []
    # at first we need to add local id in path
    path.append(local_id)
    # start a server, run it in the background
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    csci4220_hw4_pb2_grpc.add_KadImplServicer_to_server(Server(), server)
    server.add_insecure_port('[::]:' + str(my_port))
    server.start()

    for line in sys.stdin:
        line = line.strip('\n')
        stdin_list = line.split()

        # handle bootstrap
        if stdin_list[0] == 'BOOTSTRAP':
            id = client_boot_strap(stdin_list)
            print('After BOOTSTRAP(%s), k_buckets now look like:' % id)
            print_k_buckets()

        # Handle STORE
        if stdin_list[0] == 'STORE':
            key_store = int(stdin_list[1])
            value_store = stdin_list[2]
            store_helper(key_store,value_store)

        # handle find node
        if stdin_list[0] == 'FIND_NODE':
            node_find = int(stdin_list[1])
            print('Before FIND_NODE command, k-buckets are:')
            print_k_buckets()
            client_find_node(node_find)
            print('After FIND_NODE command, k-buckets are:')
            print_k_buckets()
            # clear path for new activity
            path.clear()
            path.append(local_id)

        # Handle FIND_VALUE
        if stdin_list[0] == 'FIND_VALUE':
            value_find = int(stdin_list[1])
            print('Before FIND_VALUE command, k-buckets are:')
            print_k_buckets()
            client_find_value(value_find)
            print('After FIND_VALUE command, k-buckets are:')
            print_k_buckets()
            path.clear()
            path.append(local_id)
        if stdin_list[0] == 'QUIT':
            client_quit(local_id)
