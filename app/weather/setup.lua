if(last_time == nil) then
    last_time = 0
end

if(get_msg == nil) then
    get_msg = 'GET https://api.seniverse.com/v3/weather/now.json?key=Ss7UOTAW14SJE_N3F&location=ip&language=en&unit=c\r\n\r\n'
end

if(label == nil) then
    label = lvgl.Label(nil, {
        x = 20, y = 1,
        text = string.format("NA"),
        text_color = "#133",
    })
end

if(socket == nil) then
    socket = net.open()
    net.connect_ip(socket, "116.62.81.138")
end

if(icon == nil) then
    icon_table = {}

    -- 天气现象见文档https://seniverse.yuque.com/hyper_data/api_v3/yev2c3
    -- 返回的json中，天气现象文字只有首字母是大写
    icon_table["Sunny"] = "A:" .. APPROOT .. "/icons/sunny.png"
    icon_table["Storm"] = "A:" .. APPROOT .. "/icons/storm.png"
    icon_table["Shower"] = "A:" .. APPROOT .. "/icons/shower.png"
    icon_table["Cloudy"] = "A:" .. APPROOT .. "/icons/overcast.png"
    icon_table["Overcast"] = "A:" .. APPROOT .. "/icons/overcast.png"
    icon_table["Mostly cloudy"] = "A:" .. APPROOT .. "/icons/mostly-cloudy.png"
    icon_table["Partly cloudy"] = "A:" .. APPROOT .. "/icons/mostly-cloudy.png"
    icon_table["Moderate rain"] = "A:" .. APPROOT .. "/icons/moderate-rain.png"
    icon_table["Light rain"] = "A:" .. APPROOT .. "/icons/light-rain.png"
    icon_table["Heavy rain"] = "A:" .. APPROOT .. "/icons/heavy-rain.png"
    icon = lvgl.Image(nil, {
        src = icon_table["sunny"],
        x = 2,
    })
end
