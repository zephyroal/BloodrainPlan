#ifndef MainProcedureHead
#define MainProcedureHead
//************************************
// 函数名称: BloodRain3D
// 函数说明：千里之行始于足下
// 作 成 者：何牧
// 加入日期：2011/03/23
//************************************


/**
CMainProcedure继承于CProcedure，
在App对象完成基本组件初始化后便由它全权负责
1，流程的调度；（场景，模式切换，后台加载机制）
2，输入消息的处理与分发，理论上除了加载Loading过程外，其余都交给当前为Active的Procedure
3，
**/
class CMainProcedure
{
public:
	CMainProcedure();
	~CMainProcedure();

protected:

private:

};
#endif