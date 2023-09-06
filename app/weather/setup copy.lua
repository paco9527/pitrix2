if(last_time == nil) then
    last_time = 0
end

if(get_msg == nil) then
    get_msg = 'GET https://api.seniverse.com/v3/weather/now.json?key=Ss7UOTAW14SJE_N3F&location=ip&language=zh-Hans&unit=c\r\n\r\n'
end

if(label == nil) then
    label = lvgl.Label(nil, {
        x = 20, y = 1,
        text = string.format("NA"),
        text_color = "#133",
    })
end

--[[
if(socket == nil) then
    socket = net.open()
    net.connect_ip(socket, "116.62.81.138")
    print("connect done") 
end
--]]

if(icon == nil) then
    weather_icon = lvgl.Image(nil, {
        src = "A:sunny.png",
    })
    weather_icon:set({
        src = "A:" .. APPROOT .. "/icons/sunny.png",
        align = {
            type = lvgl.ALIGN.LEFT_MID,
            x_ofs = 2
        }
    })
end
