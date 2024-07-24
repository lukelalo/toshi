//------------------------------------------------------------------------------
//  nchannelservertest.cc
//  (C) 2001 A.Weissflog
//------------------------------------------------------------------------------
#include "nebulatests/nchannelservertest.h"
#include "gfx/nchannelserver.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"

//------------------------------------------------------------------------------
/**
    Initialize the test case.
*/
void
nChannelServerTest::Initialize(nKernelServer* kernelServer)
{
    this->ks = kernelServer;
    this->chnServer = (nChannelServer*) ks->New("nchannelserver", "/sys/servers/channel");
    t_assert(this->chnServer);
}

//------------------------------------------------------------------------------
/**
    Shutdown the test case.
*/
void
nChannelServerTest::Shutdown()
{
    this->chnServer->Release();
}

//------------------------------------------------------------------------------
/**
    Run the test case.
*/
void
nChannelServerTest::Run()
{
    nChannelServer* cs = this->chnServer;
    t_assert(cs);

    int timeChn;    // a 1-dimensional time channel
    int speedChn;   // a 3-dimensional speed channel
    int colorChn;   // a 4-dimensional color channel
    int posChn;     // a 2-dimensional position channel

    // first, define a few channels
    timeChn  = cs->GenChannel("time");
    speedChn = cs->GenChannel("speed");
    colorChn = cs->GenChannel("color");
    posChn   = cs->GenChannel("pos");

    // test index lookup by name for existing channels
    t_assert(timeChn  == cs->GenChannel("time"));
    t_assert(speedChn == cs->GenChannel("speed"));
    t_assert(colorChn == cs->GenChannel("color"));
    t_assert(posChn   == cs->GenChannel("pos"));

    // define global values for channels
    const float c_time0  = 123.4f;
    const float c_pos0   = 1.2f;
    const float c_pos1   = 3.4f;
    const float c_speed0 = 100.0f;
    const float c_speed1 = 200.0f;
    const float c_speed2 = 300.0f;
    const float c_color0 = 0.1f;
    const float c_color1 = 0.2f;
    const float c_color2 = 0.3f;
    const float c_color3 = 0.4f;

    cs->SetChannel1f(timeChn,  c_time0);
    cs->SetChannel2f(posChn,   c_pos0, c_pos1);
    cs->SetChannel3f(speedChn, c_speed0, c_speed1, c_speed2);
    cs->SetChannel4f(colorChn, c_color0, c_color1, c_color2, c_color3);

    // test whether the global values made it
    float v_time0;
    float v_pos0, v_pos1;
    float v_speed0, v_speed1, v_speed2;
    float v_color0, v_color1, v_color2, v_color3;
    
    v_time0 = cs->GetChannel1f(timeChn);
    t_assert(c_time0 == v_time0);
    
    cs->GetChannel2f(posChn, v_pos0, v_pos1);
    t_assert(c_pos0 == v_pos0);
    t_assert(c_pos1 == v_pos1);

    cs->GetChannel3f(speedChn, v_speed0, v_speed1, v_speed2);
    t_assert(c_speed0 == v_speed0);
    t_assert(c_speed1 == v_speed1);
    t_assert(c_speed2 == v_speed2);

    cs->GetChannel4f(colorChn, v_color0, v_color1, v_color2, v_color3);
    t_assert(c_color0 == v_color0);
    t_assert(c_color1 == v_color1);
    t_assert(c_color2 == v_color2);
    t_assert(c_color3 == v_color3);

    // now define a few different channel sets
    nChannelSet chnSet0;
    nChannelSet chnSet1;
    nChannelSet chnSet2;

    // channel set 0 only requires time
    int chnSet0Time = chnSet0.AddChannel(cs, "time");

    // channel set 1 has time, speed and color
    int chnSet1Time  = chnSet1.AddChannel(cs, "time");
    int chnSet1Speed = chnSet1.AddChannel(cs, "speed");
    int chnSet1Color = chnSet1.AddChannel(cs, "color");

    // channel set 2 has color and pos
    int chnSet2Color = chnSet2.AddChannel(cs, "color");
    int chnSet2Pos   = chnSet2.AddChannel(cs, "pos");

    t_assert(chnSet0.GetNumChannels() == 1);
    t_assert(chnSet1.GetNumChannels() == 3);
    t_assert(chnSet2.GetNumChannels() == 2);

    // create some channel contexts for the channel sets
    cs->BeginScene();

    nChannelContext* chnSet0Context0 = cs->GetContext(&chnSet0);
    nChannelContext* chnSet0Context1 = cs->GetContext(&chnSet0);
    t_assert(chnSet0Context0);
    t_assert(chnSet0Context1);

    nChannelContext* chnSet1Context0 = cs->GetContext(&chnSet1);
    nChannelContext* chnSet1Context1 = cs->GetContext(&chnSet1);
    t_assert(chnSet1Context0);
    t_assert(chnSet1Context1);

    nChannelContext* chnSet2Context0 = cs->GetContext(&chnSet2);
    nChannelContext* chnSet2Context1 = cs->GetContext(&chnSet2);
    t_assert(chnSet2Context0);
    t_assert(chnSet2Context1);

    // test channel contents for the contexts
    v_time0 = chnSet0Context0->GetChannel1f(chnSet0Time);
    t_assert(v_time0 == c_time0);
    v_time0 = chnSet0Context1->GetChannel1f(chnSet0Time);
    t_assert(v_time0 == c_time0);

    v_time0 = chnSet1Context0->GetChannel1f(chnSet1Time);
    t_assert(v_time0 == c_time0);
    chnSet1Context0->GetChannel3f(chnSet1Speed, v_speed0, v_speed1, v_speed2);
    t_assert(v_speed0 == c_speed0);
    t_assert(v_speed1 == c_speed1);
    t_assert(v_speed2 == c_speed2);
    chnSet1Context0->GetChannel4f(chnSet1Color, v_color0, v_color1, v_color2, v_color3);
    t_assert(v_color0 == c_color0);
    t_assert(v_color1 == c_color1);
    t_assert(v_color2 == c_color2);
    t_assert(v_color3 == c_color3);

    v_time0 = chnSet1Context1->GetChannel1f(chnSet1Time);
    t_assert(v_time0 == c_time0);
    chnSet1Context1->GetChannel3f(chnSet1Speed, v_speed0, v_speed1, v_speed2);
    t_assert(v_speed0 == c_speed0);
    t_assert(v_speed1 == c_speed1);
    t_assert(v_speed2 == c_speed2);
    chnSet1Context1->GetChannel4f(chnSet1Color, v_color0, v_color1, v_color2, v_color3);
    t_assert(v_color0 == c_color0);
    t_assert(v_color1 == c_color1);
    t_assert(v_color2 == c_color2);
    t_assert(v_color3 == c_color3);

    chnSet2Context0->GetChannel4f(chnSet2Color, v_color0, v_color1, v_color2, v_color3);
    t_assert(v_color0 == c_color0);
    t_assert(v_color1 == c_color1);
    t_assert(v_color2 == c_color2);
    t_assert(v_color3 == c_color3);
    chnSet2Context0->GetChannel2f(chnSet2Pos, v_pos0, v_pos1);
    t_assert(v_pos0 == c_pos0);
    t_assert(v_pos1 == c_pos1);

    chnSet2Context1->GetChannel4f(chnSet2Color, v_color0, v_color1, v_color2, v_color3);
    t_assert(v_color0 == c_color0);
    t_assert(v_color1 == c_color1);
    t_assert(v_color2 == c_color2);
    t_assert(v_color3 == c_color3);
    chnSet2Context1->GetChannel2f(chnSet2Pos, v_pos0, v_pos1);
    t_assert(v_pos0 == c_pos0);
    t_assert(v_pos1 == c_pos1);

    // modify context channel and verify that all other
    // contexts and the global channels are untouched!
    chnSet2Context1->SetChannel4f(chnSet2Color, 1.0f, 0.99f, 0.98f, 0.97f);
    chnSet2Context1->SetChannel2f(chnSet2Pos, 50.0f, 60.0f);

    chnSet2Context0->GetChannel4f(chnSet2Color, v_color0, v_color1, v_color2, v_color3);
    t_assert(v_color0 == c_color0);
    t_assert(v_color1 == c_color1);
    t_assert(v_color2 == c_color2);
    t_assert(v_color3 == c_color3);
    chnSet2Context0->GetChannel2f(chnSet2Pos, v_pos0, v_pos1);
    t_assert(v_pos0 == c_pos0);
    t_assert(v_pos1 == c_pos1);

    chnSet2Context1->GetChannel4f(chnSet2Color, v_color0, v_color1, v_color2, v_color3);
    t_assert(v_color0 == 1.0f);
    t_assert(v_color1 == 0.99f);
    t_assert(v_color2 == 0.98f);
    t_assert(v_color3 == 0.97f);
    chnSet2Context1->GetChannel2f(chnSet2Pos, v_pos0, v_pos1);
    t_assert(v_pos0 == 50.0f);
    t_assert(v_pos1 == 60.0f);

    cs->EndScene();
}

//------------------------------------------------------------------------------




