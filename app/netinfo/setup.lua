last_time = 0
retry = 0
ret = 0

if(get_msg == nil) then
    get_msg = 'GET /api/v1/data?'..
            'chart=net.eth0&'..
            'points=1&'..
            'after=-2&'..
            'units=kilobits&'..
            'options=seconds HTTP/1.1\r\n'..
            'Connection: keep-alive\r\n'..
            'Host: 192.168.31.247:19999\r\n\r\n'
end

if(send_label == nil) then
    send_label = lvgl.Label(nil, {
        x = 1, y = 1,
        text = string.format("NA"),
        text_color = "#100",
    })
end

if(recv_label == nil) then
    recv_label = lvgl.Label(nil, {
        x = 18, y = 1,
        text = string.format("NA"),
        text_color = "#010",
    })
end