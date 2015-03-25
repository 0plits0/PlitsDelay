#include "PlitsDelay.h"
#include <math.h>
bool inst = false;


AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
	return new PlitsDelay(audioMaster);
}

PlitsDelay::PlitsDelay(audioMasterCallback audioMaster) :
AudioEffectX(audioMaster, 0, NUM_PARAMS) {
	setNumInputs(2);		// stereo in
	setNumOutputs(2);		// stereo out
	setUniqueID('PlDl');	// identify
	canMono();				// makes sense to feed both inputs with the same signal
	//canProcessReplacing();	// supports both accumulating and replacing output
	//strcpy(PlitsDelay::programName, "Default");	// default program name
	
	if (!inst){
		bufpos = 0;
		delay = 410;
		sr = getSampleRate();
		for (int i = 0; i < sr; i++){ bfl[i] = 0; bfr[i] = 0; }
		dels = floor((delay / 1000.0)*sr);
		dw = 0.45;
		fdb = 0.4;
		inst = true;

		lr = 0;
	}
}

PlitsDelay::~PlitsDelay() {
}

void PlitsDelay::open(){
}


void PlitsDelay::setParameter(VstInt32 index, float value)
{
	if (index == 0) {
		delay = (1000 * value);
		dels = floor((delay / 1000.0)*sr);
	}
	else if (index == 1) dw = (value/2);
	else if (index == 2) fdb = value;
	else if (index == 3) flzn = value;
	else if (index == 4) flzf = value;
}

float PlitsDelay::getParameter(VstInt32 index)
{
	if (index == 0) return (delay/1000);
	else if (index == 1) return (2*dw);
	else if (index == 2) return fdb;
	else if (index == 3) return flzn;
	else if (index == 4) return flzf;
}

void PlitsDelay::getParameterName(VstInt32 index, char *label)
{
	if (index == 0) strcpy(label, "Delay");
	else if (index == 1) strcpy(label, "Dry/Wet");
	else if (index == 2) strcpy(label, "Feedback");
	else if (index == 3) strcpy(label, "XAmount");
	else if (index == 4) strcpy(label, "XFreq");
}

//-----------------------------------------------------------------------------------------
void PlitsDelay::getParameterDisplay(VstInt32 index, char *text)
{
	if (index == 0) int2string(delay, text, 5);
	else if (index == 1) int2string(200*dw, text,5);
	else if (index == 2) int2string(100*fdb, text, 5);
	else if (index == 3) int2string(100 * flzn, text, 5);
	else if (index == 4) float2string(8.0 * flzf, text, 5);
}

//-----------------------------------------------------------------------------------------
void PlitsDelay::getParameterLabel(VstInt32 index, char *label)
{
	if (index == 0) strcpy(label, " ms");
	else if (index == 1) strcpy(label, " %");
	else if (index == 2) strcpy(label, " %");
	else if (index == 3) strcpy(label, " %");
	else if (index == 4) strcpy(label, " Hz");
}

//------------------------------------------------------------------------------

bool PlitsDelay::getEffectName(char* name)
{
	strcpy(name, "Delay");
	return true;
}

//------------------------------------------------------------------------
bool PlitsDelay::getProductString(char* text)
{
	strcpy(text, "PlitsDelay");
	return true;
}

//------------------------------------------------------------------------
bool PlitsDelay::getVendorString(char* text)
{
	strcpy(text, "An. Pikridas");
	return true;
}



void PlitsDelay::processReplacing(float **inputs, float **outputs,
	VstInt32 sampleFrames) {
	float *in1 = inputs[0];
	float *in2 = inputs[1];
	float *out1 = outputs[0];
	float *out2 = outputs[1];
	int td; float flp;

	for (int i = 0; i < sampleFrames; i++){
		td = (bufpos - dels>0) ? (bufpos - dels) : (sr + bufpos - dels);
		bfl[bufpos] = (1.-fdb)*in1[i] + fdb*bfl[td];
		bfr[bufpos] = (1.-fdb)*in2[i] + fdb*bfr[td];
		out1[i] = dw*bfl[td] + (1.-dw)*in1[i];
		out2[i] = dw*bfr[td] + (1.-dw)*in2[i];
		bufpos++; if (bufpos == sr) bufpos = 0;
		
		lr++;
		
		flp = flzn*20*cos(2.0 * 3.14 * (flzf*8.0)*lr / sr);
		dels = floor(((delay+flp) / 1000.0)*sr);
	}

	//for (int i = 0; i < sampleFrames; i++){
		//out1[i] = 0.6*out1[i]+0.4*out1[i]*cos(2.0 * 3.14 * (delay/10)*lr/sr);
		//out2[i] = 0.6*out1[i]+0.4*out2[i]*(1-cos(2.0 * 3.14 * (delay/10)*lr/sr));
		//lr++;
		//if (cos(2 * 3.14*(5000)*lr)-1<0.1) lr = 0;
	//}
		
}
