--[[
-----------------------------------------------------------------------------
This Source File Is Part Of [Cross Platform Utility Library]
 
Copyright (c) winslylord@qtx All Rights Reserved
 
Mail				: winslylord@me.com
 
Description			: Lua Utility
 
Target Platform		: All
 
Ver					: 1.0.0
-----------------------------------------------------------------------------
--]]

-- 命名空间
local Namespace = function( ... )

	local tbl = _G;
	table.foreachi( 
		arg, function(i,v) 
		if ( tbl[v] == nil ) then
			tbl[v] = {};
		end;
		tbl = tbl[v];
		end
		);

end;

Namespace( "Utility", "Debug" );

-- 和c printf相似的格式化输出函数
Utility.printf = function( ... )
	--print( string.format(...) )
	--Fariy.Util.PR( string.format(...) );
end;

-- 判定给定的对象是否是一个字符串
Utility.is_string = function( v )
	return type(v) == "string";
end;

-- 判定给定的对象是否是一个数字
Utility.is_number = function( v )
	return type(v) == "number";
end;

-- 判定给定的对象是否是一个函数
Utility.is_function = function( v )
	return type(v) == "function";
end;

-- 判定给定的对象是否是一个表
Utility.is_table = function( v )
	return type(v) == "table";
end;

-- 判定所有的参数是否都是数字
Utility.all_is_number = function( ... )
	for _,v in ipairs( {...} ) do
		if ( not(type(v)=="number") ) then
			return false;
		end;
	end;
	return true;
end;

-- 判定所有的参数是否都是字符串
Utility.all_is_string = function( ... )
	for _,v in ipairs( {...} ) do
		if ( not(type(v)=="string") ) then
			return false;
		end;
	end;
	return true;
end;

-- Profile_Begin
Utility.ProfileBegin = function( n )
	Fariy.Util.ProfileBegin( n );
end;

-- Profile_End
Utility.ProfileEnd = function( n )
	Fariy.Util.ProfileEnd( n );
end;

-- 输出索引开始的表内容，支持类型输出
Utility.Debug.Dump = function( tbl, ind )
	local indent=ind or 1;
	local i
	if type(tbl)=="table" then
		table.foreach(tbl,function(k,v)
			local line={}
			table.insert(line,string.rep("  ",indent))
			table.insert(line,string.format("%s=[%s]\t%s\n",k,type(v),tostring(v)));
			print(table.concat(line));
			if type(v)=="table" then
				Utility.Debug.Dump(v,indent+1)
			end
		end)
	else
		local line={}
		table.insert(line,string.format("[%s] %s\n",type(v),tostring(v)));
		print(table.concat(line));
	end
end

-- 简化宏
printf 			= Utility.printf;
is_string		= Utility.is_string;
is_number		= Utility.is_number;
is_function		= Utility.is_function;
is_table		= Utility.is_table;
all_is_string 	= Utility.all_is_string;
all_is_number 	= Utility.all_is_number;
dump_table		= Utility.Debug.Dump;
profile_begin = Utility.ProfileBegin;
profile_end = Utility.ProfileEnd;

-- 将数组从src中从索引off开始的元素复制到dst中，off默认为1
Utility.CopyArray = function( src, dst, off )
	off = off or 1;
	local i;
	for i = off,#src do
		table.insert(dst,src[i]);
	end;
end;

-- 将表src中的元素复制到dst中，src将会覆盖同名元素
Utility.CopyTable = function( src, dst )
	table.foreach( src, function(k,v)
		dst[k] = v;
		end );
end;

-- id函数
Utility.Identity = function( v )
	return v;
end;

-- 该函数将返回一个函数，该函数首次调用时返回一个可调用的对象，每个可调用对象，每次调用都返回本身，即可无限连续调用
-- 每次调用都可以接收任意个参数，并将每次调用的参数全部收集起来放入到成员args中
-- 例子 at=Utility.Accumulator("at")  at(0,360)(1,2,3) 则 at.name="at", at.args={0,360,1,2,3}
-- 将首次返回为函数而非是一个表的目的是为了防止多次使用该对象造成的所有数据累积到一个表里，即确定作用域
Utility.Accumulator = function( name )
	return function( ... )
		local ret = { name = name };
		setmetatable(ret,{
			__call=function(...)
				Utility.CopyArray(ret,arg,2)
				return ret;
			end
		})
		Utility.CopyArray(ret,arg)
		return ret;
	end;
end;

--[[
		以下部分为lua调试函数，负责打印lua调用栈等信息
		例：function fun() 
				Utility.Debug.StartTrace(); 
				print("...");
				Utility.Debug.StopTrace(); 
			end;
--]]
Utility.Debug.InvokeTracerNames	=	{}
Utility.Debug.InvokeTracer		=	{}

function Utility.Debug.InvokeTracer:__index(name)
	return function(...)
		local result={}
		table.insert(result,string.format("Utility.Debug.InvokeTracer %s(",name))
		local i
		for i=1,#arg do
			table.insert(result," "..tostring(arg[i]));
		end
		local uppers={}
		for c in string.gmatch(name,"%u") do
			table.insert(uppers,c)
		end
		local nameBase=table.concat(uppers).."_"
		local n=1+(Utility.Debug.InvokeTracerNames[nameBase] or 0)
		Utility.Debug.InvokeTracerNames[nameBase]=n;
		local returnValue=nameBase..tostring(n)

		table.insert(result," ), return="..returnValue)
		print(table.concat(result))
		return returnValue
	end
end
setmetatable(Utility.Debug.InvokeTracer,Utility.Debug.InvokeTracer)

function Utility.Debug.GetExecutionInfo(level)
	local info=debug.getinfo(level);
	local message=""
	if info.source~=nil and info.source:sub(1,1)=='@' then
		message=message.."in file "..info.source:sub(2).." "
	end
	if info.name~=nil then
		message=message.."function "..info.name.." "
	end
	message=message.."line "..tostring(info.currentline);
	if info.short_src~=nil then
		message=message.." ,short_src "..info.short_src
	end
	return message
end
function Utility.Debug.Trace()
	local message=">>Trace "..Utility.Debug.GetExecutionInfo(2)

	print (message);
end;


function Utility.Debug.DebugHook()
	local message="[Single step] "..Utility.Debug.GetExecutionInfo(3)
	print(message)
end


function Utility.Debug.StartTracing()
	debug.sethook(Utility.Debug.DebugHook,"l");
end

function Utility.Debug.StopTracing()
	debug.sethook(nil,"");
end

function getTime_s()
	return 0;
	--return Ogre.Util.gettime_s();
end