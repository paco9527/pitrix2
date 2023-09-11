cur_time = os.time()
hour = os.date("%H", cur_time)
--[[
HTTP/1.1 200 OK
Connection: keep-alive
Server: Netdata Embedded HTTP Server v1.33.1
Access-Control-Allow-Origin: *
Access-Control-Allow-Credentials: true
Content-Type: application/json; charset=utf-8
Date: Mon, 11 Sep 2023 14:29:28 UTC
Cache-Control: no-cache, no-store, must-revalidate
Pragma: no-cache
Expires: Mon, 11 Sep 2023 14:29:30 UTC
Content-Length: 97

{
 "labels": ["time", "received", "sent"],
    "data":
 [
      [ 1694442564, 0.08, -0.056]
  ]
}
]]

if(cur_time - last_time >= 2) then
    socket = net.open()
    
    if(socket ~= nil) then
        net.connect_ip(socket, "192.168.31.247", 19999)
        ret = net.timed_write(socket, get_msg)

        if(ret >= 0) then
            last_time = cur_time
            recv = net.timed_read(socket, 1024)
            if(type(recv) == "string") then
                recv_json_idx = string.find (recv, '\r\n\r\n')
                recv_json = string.sub(recv, recv_json_idx+4) -- 跳过\r\n\r\n

                recv_decode = cjson.decode(recv_json)

                send_kps = math.floor(recv_decode["data"][1][3])
                if(send_kps < 0) then
                    send_kps = -send_kps
                end

                if(send_kps < 1000) then
                    string = string.sub(tostring(send_kps), 1, 2)
                    send_label:set({ text = string.format("%dK", string) })
                else
                    string = string.sub(tostring(send_kps/1000), 1, 2)
                    send_label:set({ text = string.format("%dM", string) })
                end

                recv_kps = math.floor(recv_decode["data"][1][2])
                if(recv_kps < 1000) then
                    string = string.sub(tostring(recv_kps), 1, 2)
                    recv_label:set({ text = string.format("%dK", string) })
                else
                    string = string.sub(tostring(recv_kps/1000), 1, 2)
                    recv_label:set({ text = string.format("%dM", string) })
                end
            end
        end
    end
    net.close(socket)
    socket = nil
end