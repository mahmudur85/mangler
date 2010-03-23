#include <jni.h>
#include <stdint.h>
#include "ventrilo3.h"
#include "debug.h"

JNIEXPORT jboolean JNICALL Java_org_mangler_VentriloInterface_recv() {
	_v3_net_message *msg = _v3_recv(V3_BLOCK);
	return msg && _v3_process_message(msg) == V3_OK;
	
}

JNIEXPORT jint JNICALL Java_org_mangler_VentriloInterface_pcmlengthforrate(JNIEnv* env, jobject obj, jint rate) {
	return v3_pcmlength_for_rate(rate);
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_joinchat() {
	v3_join_chat();
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_leavechat() {
	v3_leave_chat();
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_logout() {
	v3_logout();
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_phantomadd(jchar channelid) {
	v3_phantom_add(channelid);
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_phantomremove(jchar channelid) {
	v3_phantom_remove(channelid);
}

JNIEXPORT jboolean JNICALL Java_org_mangler_VentriloInterface_isloggedin() {
	return v3_is_loggedin() != 0;
}

JNIEXPORT jchar JNICALL Java_org_mangler_VentriloInterface_getuserid() {
	return v3_get_user_id();
}

JNIEXPORT jint JNICALL Java_org_mangler_VentriloInterface_messagewaiting(jint block) {
	return v3_message_waiting(block);
}

JNIEXPORT jint JNICALL Java_org_mangler_VentriloInterface_getmaxclients() {
	return v3_get_max_clients();
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_clearevents() {
	v3_clear_events();
}

JNIEXPORT jint JNICALL Java_org_mangler_VentriloInterface_getcodecrate(jchar codec, jchar format) {
	return v3_get_codec_rate(codec, format);
}

JNIEXPORT jchar JNICALL Java_org_mangler_VentriloInterface_getuserchannel(jchar id) {
	return v3_get_user_channel(id);
}

JNIEXPORT jchar JNICALL Java_org_mangler_VentriloInterface_channelrequirespassword(jchar channelid) {
	return v3_channel_requires_password(channelid);
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_startaudio(jchar sendtype) {
	v3_start_audio(3);
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_stopaudio() {
	v3_stop_audio();
}

JNIEXPORT jint JNICALL Java_org_mangler_VentriloInterface_usercount() {
	return v3_user_count();
}

JNIEXPORT jint JNICALL Java_org_mangler_VentriloInterface_channelcount() {
	return v3_channel_count();
}

JNIEXPORT int JNICALL Java_org_mangler_VentriloInterface_debuglevel(jint level) {
	return v3_debuglevel(level);
}

JNIEXPORT jint JNICALL Java_org_mangler_VentriloInterface_login(JNIEnv* env, jobject obj, jstring server, jstring username, jstring password, jstring phonetic) {
	v3_debuglevel(V3_DEBUG_INTERNAL);
	char* _server	= (char*)(*env)->GetStringUTFChars(env, server, 0);
	char* _username = (char*)(*env)->GetStringUTFChars(env, username, 0);
	char* _password = (char*)(*env)->GetStringUTFChars(env, password, 0);
	char* _phonetic = (char*)(*env)->GetStringUTFChars(env, phonetic, 0);
	jint ret = v3_login(_server, _username, _password, _phonetic);
	(*env)->ReleaseStringUTFChars(env, server, _server);
	(*env)->ReleaseStringUTFChars(env, username, _username);
	(*env)->ReleaseStringUTFChars(env, password, _password);
	(*env)->ReleaseStringUTFChars(env, phonetic, _phonetic);
	return ret;
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_sendchatmessage(JNIEnv* env, jobject obj, jstring message) {
	char* _message = (char*)(*env)->GetStringUTFChars(env, message, 0);
	v3_send_chat_message(_message);
	(*env)->ReleaseStringUTFChars(env, message, _message);
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_changechannel(JNIEnv* env, jobject obj, jchar channelid, jstring password) {
	char* _password = (char*)(*env)->GetStringUTFChars(env, password, 0);
	v3_change_channel(channelid, _password);
	(*env)->ReleaseStringUTFChars(env, password, _password);
}


JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_settext(JNIEnv* env, jobject obj, jstring comment, jstring url, jstring integrationtext, jboolean silent) {
	char* _comment = (char*)(*env)->GetStringUTFChars(env, comment, 0);
	char* _url = (char*)(*env)->GetStringUTFChars(env, url, 0);
	char* _integrationtext = (char*)(*env)->GetStringUTFChars(env, integrationtext, 0);
	v3_set_text(_comment, _url, _integrationtext, silent);
	(*env)->ReleaseStringUTFChars(env, comment, _comment);
	(*env)->ReleaseStringUTFChars(env, url, _url);
	(*env)->ReleaseStringUTFChars(env, integrationtext, _integrationtext);
}

JNIEXPORT jint JNICALL Java_org_mangler_VentriloInterface_getevent(JNIEnv* env, jobject obj, jobject eventdata) {
	v3_event *ev = v3_get_event(V3_BLOCK);
	if(ev != NULL) {
		jclass event_class = (*env)->GetObjectClass(env, eventdata);

		// Event type.
		(*env)->SetShortField(
			env, 
			eventdata, 
			(*env)->GetFieldID(env, event_class, "type", "S"), 
			ev->type
		);
		
		switch(ev->type) {
			case V3_EVENT_PLAY_AUDIO:
				{
					// Grab PCM object.
					jobject pcm = 
					(*env)->GetObjectField(
						env, 
						eventdata, 
						(*env)->GetFieldID(env, event_class, "pcm", "Lorg/mangler/VentriloEventData$_pcm;")
					);
					jclass pcm_class = (*env)->GetObjectClass(env, pcm);
					
					// Set PCM fields.
					(*env)->SetIntField(
						env, 
						pcm, 
						(*env)->GetFieldID(env, pcm_class, "length", "I"), 
						ev->pcm.length
					);
					(*env)->SetShortField(
						env, 
						pcm, 
						(*env)->GetFieldID(env, pcm_class, "send_type", "S"), 
						ev->pcm.send_type
					);
					(*env)->SetIntField(
						env, 
						pcm, 
						(*env)->GetFieldID(env, pcm_class, "rate", "I"), 
						ev->pcm.rate
					);
					(*env)->SetByteField(
						env, 
						pcm, 
						(*env)->GetFieldID(env, pcm_class, "channels", "B"), 
						ev->pcm.channels
					);
					
					// Grab user object.
					jobject user = 
					(*env)->GetObjectField(
						env, 
						eventdata, 
						(*env)->GetFieldID(env, event_class, "user", "Lorg/mangler/VentriloEventData$_user;")
					);
					jclass user_class = (*env)->GetObjectClass(env, user);
					
					// Set user field(s).
					(*env)->SetShortField(
						env, 
						user, 
						(*env)->GetFieldID(env, user_class, "id", "S"), 
						ev->user.id
					);
					
					// Grab data object.
					jobject data = 
					(*env)->GetObjectField(
						env, 
						eventdata, 
						(*env)->GetFieldID(env, event_class, "data", "Lorg/mangler/VentriloEventData$_data;")
					);
					jclass data_class = (*env)->GetObjectClass(env, data);
					
					// Set sample.
					(*env)->SetByteArrayRegion(
						env, 
						(*env)->GetObjectField(
							env,
							data,
							(*env)->GetFieldID(env, data_class, "sample", "[B")
						), 
						0, 
						ev->pcm.length, 
						ev->data->sample
					);
					 
				}
				break;
		}
		
		free(ev);
	}
	return 0;
}

JNIEXPORT void JNICALL Java_org_mangler_VentriloInterface_sendaudio(JNIEnv* env, jobject obj, jbyteArray pcm, jint size, jint rate) {
	jboolean isCopy;
	jbyte *data = (*env)->GetByteArrayElements(env, pcm, &isCopy);
	v3_send_audio(V3_AUDIO_SENDTYPE_U2CCUR, rate, data, size, 0);
	(*env)->ReleaseByteArrayElements(env, pcm, data, 0);
}