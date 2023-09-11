cur_time = os.time()
hour = os.date("%H", cur_time)

--[[
{
    "results":
    [
        {
            "location":
            {
                "id":"WTMKQ069CCJ7",
                "name":"杭州",
                "country":"CN",
                "path":"杭州,杭州,浙江,中国",
                "timezone":"Asia/Shanghai",
                "timezone_offset":"+08:00"
            },
            "now":
            {
                "text":"多云",
                "code":"4",
                "temperature":"29"
            },
            "last_update":"2023-07-01T09:34:31+08:00"
        }
    ]
}
]]
if(cur_time - last_time >= 1800) then
    socket = net.open()
    
    if(socket ~= nil) then
        net.connect_ip(socket, "116.62.81.138") -- 心知天气IP
        ret = net.timed_write(socket, get_msg)
        -- 发送成功才能继续
        if(ret >= 0) then
            last_time = cur_time
        
            recv = net.timed_read(socket, 1024)
            if(type(recv) == "string") then
                -- 接收正常才能继续
                result = cjson.decode(recv)
                city = result["results"][1]["now"]
                temperature = city["temperature"] -- 气温
                weather_text = city["text"] -- 天气现象代码参考 https://seniverse.yuque.com/hyper_data/api_v3/yev2c3
                -- print(weather_text) -- 打印天气现象字符串，如果有时看不到天气图标，查找setup.lua里有没有和这个字符串对应的图标
                label:set({ text = tostring(temperature) })
                if((weather_text == "Fair") 
                    or (weather_text == "Mostly cloudy") 
                    or (weather_text == "Partly cloudy")) then
                    if((hour > 18) or (hour < 6)) then
                        icon:set({
                            src = icon_table[weather_text .. "night"]
                        })
                    else
                        icon:set({
                            src = icon_table[weather_text]
                        })
                    end
                else
                    icon:set({
                        src = icon_table[weather_text]
                    })
                end
                recv = nil
            else
                retry = retry + 1
            end
        end
        net.close(socket)
        socket = nil
    end
end


