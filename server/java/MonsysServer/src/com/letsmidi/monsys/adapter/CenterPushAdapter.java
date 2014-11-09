package com.letsmidi.monsys.adapter;

import com.letsmidi.monsys.protocol.center.Center;
import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.Channel;

import java.util.HashMap;

/**
 * Created by zero on 9/19/14.
 */
public class CenterPushAdapter {
  public static class InnerAddress {
    private Channel channel;
  }

  private final Channel mChannel;

  // TODO: make the initial size of map as a config item
  private HashMap<Integer, InnerAddress> mRouteRecords = new HashMap<>(100);

  public CenterPushAdapter(Channel channel) {
    mChannel = channel;
  }

  /**
   * Conver from center message to push message, then send to peer
   * @param address  The inner address
   * @param msg      the message should be processed
   */
  public void send(InnerAddress address, Center.CenterMsg msg) {
    // generate a sequence id
    int sequence_id = 0;

    // new message
    Push.PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(Push.MsgType.BIND);
    builder.setSequence(sequence_id);

    mChannel.writeAndFlush(builder.build());
  }
}
