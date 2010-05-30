package org.mangler;

import android.app.TabActivity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TabHost;

import java.util.HashMap;

public class ServerView extends TabActivity {
	
	private HashMap<Short, String> channels;
	private HashMap<Short, String> users;
	
	private boolean  stopEventThread;
	private String 	 hostname;
	private int 	 port;
	private String 	 password;
	private String 	 username;
	private String 	 phonetic;
	private Player 	 player;
	private Recorder recorder;
	
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
        setContentView(R.layout.server_view);

        channels = new HashMap<Short, String>();
        channels.put((short)0, "Lobby");
        
        users = new HashMap<Short, String>();

        TabHost tabHost = getTabHost();
        TabHost.TabSpec tabSpec;
        Intent intent;
        
    	Bundle extras = getIntent().getExtras();
    	if (extras != null) {
    		hostname = extras.getString("hostname");
    		port = extras.getInt("port");
    		password = extras.getString("password");
    		username = extras.getString("username");
    		phonetic = extras.getString("phonetic");
    		
    		connect(hostname, port, password, username, phonetic);
    	}
 
    	final Button ttalk = (Button)findViewById(R.id.ToggleTalkButton);
    	ttalk.setText(R.string.start_talk);
        
        ttalk.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				if (!recorder.recording()) {
					recorder.start();
					ttalk.setText(R.string.stop_talk);
				} else {
					recorder.stop();
					ttalk.setText(R.string.start_talk);
				}
			}
        });
    	  	
    	tabSpec = tabHost.newTabSpec("talk").setIndicator("Talk").setContent(R.id.talkView);
        tabHost.addTab(tabSpec);
    	
        intent = new Intent().setClass(this, ChannelList.class);
        intent.putExtra("channels", channels);

        tabSpec = tabHost.newTabSpec("channels").setIndicator("Channels").setContent(intent);
        tabHost.addTab(tabSpec);

        intent = new Intent().setClass(this, UserList.class);
        intent.putExtra("users", users);
        
        tabSpec = tabHost.newTabSpec("users").setIndicator("Users").setContent(intent);
        tabHost.addTab(tabSpec);
        
        tabHost.setCurrentTab(0);
    }
    
    @Override
    protected void onStop() {
    	super.onStop();
    	recorder.stop();
    	stopEventThread = true;
    	VentriloInterface.logout();
    }
    
    @Override
    protected void onRestart() {
    	super.onRestart();
    	stopEventThread = false;
    	connect(hostname, port, password, username, phonetic);
    }
    
    private String StringFromBytes(byte[] bytes) {
    	return new String(bytes, 0, (new String(bytes).indexOf(0)));
    }
    
    protected void connect(String hostname, int port, String password, String username, String phonetic) {
    	// We need to start this before calling v3_login
    	startEventThread();
    	
    	VentriloInterface.debuglevel(VentriloDebugLevels.V3_DEBUG_INFO);
        if(VentriloInterface.login(hostname + ":" + port, username, password, phonetic)) {
	    	startRecvThread();
        }
    }
    
    private void startEventThread() {
    	Runnable event_runnable = new Runnable() {
    		public void run() {
		    	VentriloEventData data = new VentriloEventData();
		    	while(true) {
		    		VentriloInterface.getevent(data);
		    		switch(data.type) {
		    			case VentriloEvents.V3_EVENT_CHAT_MESSAGE:
		    				Log.i("mangler", "User sent message: " + Integer.toString(data.user.id) + " -> " + StringFromBytes(data.data.chatmessage));
		    				break;
		    				
		    			case VentriloEvents.V3_EVENT_CHAT_JOIN:
		    				Log.i("mangler", "User joined chat: " + Integer.toString(data.user.id));
		    				break;
		    			
		    			case VentriloEvents.V3_EVENT_PING:
		    				Log.i("mangler", "Ping: " + Integer.toString(data.ping));
		    				break;
		    			
		    			case VentriloEvents.V3_EVENT_USER_TALK_START:
		    				Log.i("mangler", "User started talking: " + Integer.toString(data.user.id) + " - Rate: " + Integer.toString(data.pcm.rate));
		    				break;
		    				
		    			case VentriloEvents.V3_EVENT_PLAY_AUDIO:
		    					player.write(data.data.sample, data.pcm.length);
		    				break;
		    			
		    			case VentriloEvents.V3_EVENT_USER_TALK_END:
		    				Log.i("mangler", "User stopped talking: " + Integer.toString(data.user.id));
			    			break;
			    			
		    			case VentriloEvents.V3_EVENT_STATUS:
		    				Log.i("mangler", "Status: " + Integer.toString(data.status.percent) + "%" + " -> " + StringFromBytes(data.status.message));
		    				break;
		    				
		    			case VentriloEvents.V3_EVENT_CHAN_ADD:
		    				VentriloInterface.getchannel(data, data.channel.id);
		    				channels.put(data.channel.id, StringFromBytes(data.text.name));
		    				
		    				Log.i("mangler", "Channel added / modified: " + Short.toString(data.channel.id) + " -> " + StringFromBytes(data.text.name));
		    				break;
		    			case VentriloEvents.V3_EVENT_CHAN_MODIFY:
		    				VentriloInterface.getchannel(data, data.channel.id);
		    				Log.i("mangler", "Channel added / modified: " + Short.toString(data.channel.id) +" -> " + StringFromBytes(data.text.name));
		    				break;
		    				
		    			case VentriloEvents.V3_EVENT_USER_LOGIN:
		    				VentriloInterface.getuser(data, data.user.id);
		    				users.put(data.user.id, StringFromBytes(data.text.name));
		    				
		    				Log.i("mangler", "User logged in: " + Short.toString(data.user.id) + " -> " + StringFromBytes(data.text.name));
		    				break;
		    				
		    			case VentriloEvents.V3_EVENT_LOGIN_COMPLETE:
		    				int lobby_rate = VentriloInterface.getchannelrate((short)0);
		    				player 	 = new Player(lobby_rate);
		    				recorder = new Recorder(lobby_rate);
		    				
		    				Log.i("mangler", "Login complete!");
		    				break;
		    				
		    			case VentriloEvents.V3_EVENT_DISPLAY_MOTD:
		    				Log.i("mangler", "MOTD: " + StringFromBytes(data.data.motd));
		    				break;
		    				
		    			case VentriloEvents.V3_EVENT_USER_CHAN_MOVE:
		    				VentriloInterface.getchannel(data, data.channel.id);
	    					if(data.user.id == VentriloInterface.getuserid()) {
	    						// Set new rate on our player and recorder.
	    						int channel_rate = VentriloInterface.getcodecrate(data.data.channel.channel_codec, data.data.channel.channel_format);
	    						player.rate(channel_rate);
	    						recorder.stop();
	    						recorder = new Recorder(channel_rate);
	    					}
	    					
	    					Log.i("mangler", "User joined channel: " + Integer.toString(data.user.id) + " - channel: " + Integer.toString(data.channel.id));
		    				break;
		    				
		    			case VentriloEvents.V3_EVENT_USER_LOGOUT:
		    				Log.i("mangler", "User left server: " + Integer.toString(data.user.id));
		    				break;
		    				
		    			case VentriloEvents.V3_EVENT_CHAT_LEAVE:
		    				Log.i("mangler", "User left chat: " + Integer.toString(data.user.id));
		    				break;
		    			
		    			default:
		    				Log.w("mangler", "Unknown event of type: " + Integer.toString(data.type));
		    		}
		    		
		    		if (stopEventThread) break;
		    	}
    		}
    	};
    	
    	(new Thread(event_runnable)).start();
    }
    
    private void startRecvThread() {
    	Runnable recv_runnable = new Runnable() {
    		public void run() {
    			while(true) {
    				if(!VentriloInterface.recv()) break;
    			}
    		}
    	};
    	
    	(new Thread(recv_runnable)).start();
    }
}