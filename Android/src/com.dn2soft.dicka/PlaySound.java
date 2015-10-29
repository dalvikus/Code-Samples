package com.dn2soft.dicka;

import java.util.zip.ZipFile;
import java.util.zip.ZipException;
import java.util.zip.ZipEntry;

import android.util.Log;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetFileDescriptor;

import java.net.URL;
import java.net.URLConnection;
import java.io.File;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.BufferedInputStream;

import android.media.AudioManager;
import android.media.MediaPlayer;

public class PlaySound {
    private static final String TAG = "PlaySound";

    public static void play(final Activity activity, final String audioPath, final String audioURLStr)
    {
        if (audioURLStr == null || audioURLStr.isEmpty())
            return;
        new Thread(new Runnable() {
            public void run() {
                if (audioPath == null) {
                    return;
                }
                // audioURLStr
                //  ogg:
                //      http://dictionary.cambridge.org/media/american-english/us_pron_ogg/t/toy/toy__/toy.ogg
                //  mp3:
                //      726419.mp3
                String audio0 = audioURLStr.substring(audioURLStr.lastIndexOf('/') + 1);
////            Log.i(TAG, "audio0: \"" + audio0 + "\"");
                // audio0
                //  ogg:
                //      toy.ogg
                //  mp3:
                //      726419.mp3
                String fmt = audio0.substring(audio0.lastIndexOf('.') + 1);
////            Log.i(TAG, "fmt: \"" + fmt + "\"");
                File audioFile = null;
                // in assets
                AssetFileDescriptor afd = null;
                String assets_audioFile = "dicka/" + fmt + "/" + audio0;
                try {
                    afd = ((Context) activity).getAssets().openFd(assets_audioFile);
                } catch (IOException e) {
                    Log.e(TAG, "play: AssetManager.openFd(\"" + assets_audioFile + "\"): IOException");
                }
                if (afd == null) {
                    // in local directory
                    audioFile = new File(audioPath, audio0);
                    // check file: exist and non-zero
                    if (!audioFile.isFile() || audioFile.length() == 0) {   // download
                        // zip file
                        String copiedStr = null;
                        try {
                            if (fmt.equals("ogg")) {
                                if (LoadActivity.mZipFileExOgg == null) {
                                    Log.e(TAG, "LoadActivity.mZipFileExOgg is null");
                                } else {
                                    copiedStr = LoadActivity.mZipFileExOgg.copy(null, audio0, audioFile.getAbsolutePath());
                                }
                            } else {    // "mp3"
                                if (LoadActivity.mZipFileExMp3 == null) {
                                    Log.e(TAG, "LoadActivity.mZipFileExMp3 is null");
                                } else {
                                    copiedStr = LoadActivity.mZipFileExMp3.copy(null, audio0, audioFile.getAbsolutePath());
                                }
                            }
                        }
                        catch (FileNotFoundException e) {e.printStackTrace();}
                        catch (ZipException e) {e.printStackTrace();}
                        catch (IOException e) {e.printStackTrace();}
Log.i(TAG, "copiedStr = " + (copiedStr == null ? "null" : ("|" + copiedStr + "|")));
/*
                        ZipEntry ze = null;
                        try {
                            ZipFile zf = new ZipFile(new File(audioPath, fmt + ".zip"));
                            ze = zf.getEntry(audio0);
                            if (ze != null) {
                                ZipFileEx.extractFileFromZipFile(zf, ze, audioFile);
                            }
                            zf.close();
////                        Log.i(TAG, "zip...");
                        }
                        catch (ZipException e) {
                            // ZipFile(String)
                            e.printStackTrace();
                        }
                        catch (IllegalStateException e) {
                            // ZipFile.getEntry
                            e.printStackTrace();
                        }
                        catch (IOException e) {
                            // ZipFile(String)
                            // ZipFile.close()
                            e.printStackTrace();
                        }

                        if (ze == null) {
 */
                        if (copiedStr == null) {
                            Log.i(TAG, "download...");
                            URL audioURL = null;
                            try {
                                audioURL = new URL(audioURLStr);
                            } catch (java.net.MalformedURLException e) {
                                e.printStackTrace();
                                audioURL = null;
                            }
                            if (audioURL == null) {
                                return;
                            }
                            try {
                                BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(audioFile));
                                URLConnection ucon = audioURL.openConnection();
                                InputStream is = ucon.getInputStream();
                                BufferedInputStream bis = new BufferedInputStream(is);
                                final int BUF_LEN = 8192;
                                byte[] buffer = new byte[BUF_LEN];
                                int byteRead;
                                while ((byteRead = bis.read(buffer, 0, BUF_LEN)) != -1) {
                                    bos.write(buffer, 0, byteRead);
                                }
                                bis.close();
                                bos.close();
                            } catch (java.io.IOException e) {
                                e.printStackTrace();
                                return;
                            }
                        }
//                      }
                    } else {
                        Log.i(TAG, "in local directory");
                    }
                } else {
                    Log.i(TAG, "in assets");
                }
                try {
/*
Default constructor. Consider using one of the create() methods for synchronously instantiating a MediaPlayer from a Uri or resource.

When done with the MediaPlayer, you should call release(), to free the resources. If not released, too many MediaPlayer instances may result in an exception.
 */
                    MediaPlayer mediaPlayer = new MediaPlayer();
//Log.i(TAG, "MediaPlayer: " + mediaPlayer);
/*
If any of these methods is called right after a MediaPlayer object is constructed, the user supplied callback method OnErrorListener.onError() won't be called by the internal player engine and the object state remains unchanged; but if these methods are called right after reset(), the user supplied callback method OnErrorListener.onError() will be invoked by the internal player engine and the object will be transfered to the Error state.
 */
                    mediaPlayer.setOnErrorListener(
                        new android.media.MediaPlayer.OnErrorListener() {
/*
True if the method handled the error, false if it didn't. Returning false, or not having an OnErrorListener at all, will cause the OnCompletionListener to be called.
 */
                            @Override
                            public boolean onError(MediaPlayer mp, int what, int extra) {
//Log.i(TAG, "onError: MediaPlayer: " + mp);
//Log.i(TAG, "onError: what: " + what);
//Log.i(TAG, "onError: extra: " + extra);
                                return false;
                            }
                        }
                    );

/*
    Must call this method before prepare() or prepareAsync() in order for the target stream type to become effective thereafter.
 */
                    mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);

/*
setDataSource():
    Sets the data source (FileDescriptor) to use. The FileDescriptor must be seekable (N.B. a LocalSocket is not seekable). It is the caller's responsibility to close the file descriptor. It is safe to do so as soon as this call returns.
 */
                    if (afd == null) {
                        FileInputStream fis = new FileInputStream(audioFile);
                        mediaPlayer.setDataSource(fis.getFD());
                        fis.close();
                    } else {
                        mediaPlayer.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
                        afd.close();
                    }
/*
prepare()
    Prepares the player for playback, synchronously. After setting the datasource and the display surface, you need to either call prepare() or prepareAsync(). For files, it is OK to call prepare(), which blocks until MediaPlayer is ready for playback.
prepareAsync()
    Prepares the player for playback, asynchronously. After setting the datasource and the display surface, you need to either call prepare() or prepareAsync(). For streams, you should call prepareAsync(), which returns immediately, rather than blocking until enough data has been buffered.
 */
                    mediaPlayer.prepare();
                    mediaPlayer.start();
                    mediaPlayer.setVolume(1.f, 1.f);
                    mediaPlayer.setOnPreparedListener(
                        new android.media.MediaPlayer.OnPreparedListener() {
                            @Override
                            public void onPrepared(MediaPlayer mp) {
//Log.i(TAG, "onPrepared: MediaPlayer: " + mp);
                                mp.start();
                            }
                        }
                    );
////                mediaPlayer.prepareAsync();
                    mediaPlayer.setOnSeekCompleteListener(
                        new android.media.MediaPlayer.OnSeekCompleteListener() {
                            @Override
                            public void onSeekComplete(MediaPlayer mp) {
//Log.i(TAG, "onSeekComplete: MediaPlayer: " + mp);
                            }
                        }
                    );
                    mediaPlayer.setOnCompletionListener(
                        new android.media.MediaPlayer.OnCompletionListener() {
                            @Override
                            public void onCompletion(MediaPlayer mp) {
//Log.i(TAG, "onCompletion: MediaPlayer: " + mp);
                                mp.release();
                                mp = null;
                            }
                        }
                    );


                    mediaPlayer.setOnBufferingUpdateListener(
                        new android.media.MediaPlayer.OnBufferingUpdateListener() {
                            @Override
                            public void onBufferingUpdate (MediaPlayer mp, int percent)
                            {
//Log.i(TAG, "onBufferingUpdate: MediaPlayer: " + mp);
//Log.i(TAG, "onBufferingUpdate: percent: " + percent);
                            }
                        }
                    );
                    mediaPlayer.setOnInfoListener(
                        new android.media.MediaPlayer.OnInfoListener() {
                            @Override
                            public boolean onInfo (MediaPlayer mp, int what, int extra)
                            {
//Log.i(TAG, "onInfo: MediaPlayer: " + mp);
//Log.i(TAG, "onInfo: what: " + what);
//Log.i(TAG, "onInfo: extra: " + extra);
/*
    True if the method handled the info, false if it didn't. Returning false, or not having an OnErrorListener at all, will cause the info to be discarded.
 */
                                return false;
                            }
                        }
                    );
/*
                    mediaPlayer.setOnTimedTextListener(
                        new android.media.MediaPlayer.OnTimedTextListener() {
                            @Override
                            public void onTimedText (MediaPlayer mp, android.media.TimedText text)
                            {
//Log.i(TAG, "onTimedText: MediaPlayer: " + mp);
//Log.i(TAG, "onTimedText: TimedText: " + text);
                            }
                        }
                    );
 */
                    mediaPlayer.setOnVideoSizeChangedListener(
                        new android.media.MediaPlayer.OnVideoSizeChangedListener() {
                            @Override
                            public void onVideoSizeChanged (MediaPlayer mp, int width, int height)
                            {
//Log.i(TAG, "onVideoSizeChanged: MediaPlayer: " + mp);
//Log.i(TAG, "onVideoSizeChanged: width = " + width);
//Log.i(TAG, "onVideoSizeChanged: height = " + height);
                            }
                        }
                    );
                } catch (IOException e) {
                    e.printStackTrace();
                    return;
                }
            }
        }).start();
    }
}
