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

-- �����ռ�
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

-- ��c printf���Ƶĸ�ʽ���������
Utility.printf = function( ... )
	--print( string.format(...) )
	--Fariy.Util.PR( string.format(...) );
end;

-- �ж������Ķ����Ƿ���һ���ַ���
Utility.is_string = function( v )
	return type(v) == "string";
end;

-- �ж������Ķ����Ƿ���һ������
Utility.is_number = function( v )
	return type(v) == "number";
end;

-- �ж������Ķ����Ƿ���һ������
Utility.is_function = function( v )
	return type(v) == "function";
end;

-- �ж������Ķ����Ƿ���һ����
Utility.is_table = function( v )
	return type(v) == "table";
end;

-- �ж����еĲ����Ƿ�������
Utility.all_is_number = function( ... )
	for _,v in ipairs( {...} ) do
		if ( not(type(v)=="number") ) then
			return false;
		end;
	end;
	return true;
end;

-- �ж����еĲ����Ƿ����ַ���
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

-- ���������ʼ�ı����ݣ�֧���������
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

-- �򻯺�
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

-- �������src�д�����off��ʼ��Ԫ�ظ��Ƶ�dst�У�offĬ��Ϊ1
Utility.CopyArray = function( src, dst, off )
	off = off or 1;
	local i;
	for i = off,#src do
		table.insert(dst,src[i]);
	end;
end;

-- ����src�е�Ԫ�ظ��Ƶ�dst�У�src���Ḳ��ͬ��Ԫ��
Utility.CopyTable = function( src, dst )
	table.foreach( src, function(k,v)
		dst[k] = v;
		end );
end;

-- id����
Utility.Identity = function( v )
	return v;
end;

-- �ú���������һ���������ú����״ε���ʱ����һ���ɵ��õĶ���ÿ���ɵ��ö���ÿ�ε��ö����ر���������������������
-- ÿ�ε��ö����Խ������������������ÿ�ε��õĲ���ȫ���ռ��������뵽��Աargs��
-- ���� at=Utility.Accumulator("at")  at(0,360)(1,2,3) �� at.name="at", at.args={0,360,1,2,3}
-- ���״η���Ϊ����������һ������Ŀ����Ϊ�˷�ֹ���ʹ�øö�����ɵ����������ۻ���һ�������ȷ��������
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
		���²���Ϊlua���Ժ����������ӡlua����ջ����Ϣ
		����function fun() 
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