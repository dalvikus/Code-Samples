package com.dn2soft.dicka;
import com.dn2soft.dicka.FolderExplorer.EntryEx;
import com.dn2soft.dicka.ZipFileExIteration.FileElement;
import com.dn2soft.dicka.ZipFileExIteration.ZipFileExFileFilter;


import android.app.Activity;

import android.content.Context;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import android.os.AsyncTask;

import android.util.Log;
import android.util.SparseBooleanArray;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;


import java.io.IOException;

import java.util.List;
import java.util.ArrayList;



class FolderAdapter
    extends
        ArrayAdapter<EntryEx>
    implements
        CompoundButton.OnCheckedChangeListener,
        View.OnClickListener
{
    private static final String TAG = "FolderAdapter";

    static class Item
    {
        String mPathname;
        int mType;
        public Item(int type, String pathname) {
            mPathname = pathname;
            mType = type;
        }
    }

    private final LayoutInflater mInflater;

    final private Bitmap mZipIcon;
    final private Bitmap mMp3Icon;
    final private Bitmap mFolderIcon;

    boolean mSelectionMode = false;
    public boolean isSelectionMode() {return mSelectionMode;}
    public void setSelectionModeOff() {
        setSelectionMode(
            false,
            -1      // any: not used
        );
    }
    // position < 0 (-1): although entering selection mode,
    // the started position is not selectable
    public void setSelectionMode(boolean selectionMode, int position) {
        mSelectionMode = selectionMode;
        mCheckedStates.clear();
        mMenuLayout.setVisibility(selectionMode ? View.VISIBLE : View.GONE);
        mStatusView.setVisibility(selectionMode ? View.VISIBLE : View.GONE);
        mUpButton.setVisibility(View.VISIBLE);
        mAllButton.setImageBitmap(mSelectAllIcon);
        mRecursiveCheckBox.setChecked(false);
        if (selectionMode) {
            if (position >= 0 && position < getCount())
                mCheckedStates.put(position, true);
        }
        notifyDataSetChanged(); // redraw ListView with CheckBox enabled
    }

    boolean mAllSelected = false;
    public boolean getAllSelected() {return mAllSelected;}
    public void toggleAllSelected() {
        if (!mSelectionMode)
            return;
        mAllSelected = !mAllSelected;
        mCheckedStates.clear();
        if (mAllSelected) {
            for (int position = 0; position < getCount(); ++position) {
                EntryEx item = (EntryEx) getItem(position);
                if (item.isSelectable())
                    mCheckedStates.put(position, true);
            }
        }
        mAllButton.setImageBitmap(!mAllSelected ? mSelectAllIcon : mDeselectAllIcon);
        notifyDataSetChanged(); // redraw ListView with CheckBox status
    }

    private SparseBooleanArray mCheckedStates = new SparseBooleanArray();
    public SparseBooleanArray getCheckedStates() {return mCheckedStates;}

    private Bitmap mSelectAllIcon;
    private Bitmap mDeselectAllIcon;
    private LinearLayout mMenuLayout;
    private TextView mStatusView;
    private ImageButton mUpButton;
    private ImageButton mAllButton;
    private ImageButton mAddButton;
    private CheckBox mRecursiveCheckBox;
    private TextView mRecursiveTextView;
    final Activity mActivity;
    final FileBrowserDialogFragment mFileBrowserDialogFragment;

    public FolderAdapter(Activity activity, FileBrowserDialogFragment fileBrowserListFragment, LinearLayout menuLayout, TextView statusView)
    {
        // android.R.layout.simple_list_item_2 is meaningless,
        // because it is used in a "default" getView, but it will be overrided here
        //  see the source of ArrayAdapter
        super((Context) activity, android.R.layout.simple_list_item_2);
        Context context = (Context) activity;

        mActivity = activity;
        mFileBrowserDialogFragment = fileBrowserListFragment;

        mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mZipIcon = BitmapFactory.decodeResource(context.getResources(), R.drawable.zip);
        mMp3Icon = BitmapFactory.decodeResource(context.getResources(), R.drawable.mp3);
        mFolderIcon = BitmapFactory.decodeResource(context.getResources(), R.drawable.ic_action_collection);
        mSelectAllIcon = BitmapFactory.decodeResource(context.getResources(), R.drawable.ic_action_accept);
        mDeselectAllIcon = BitmapFactory.decodeResource(context.getResources(), R.drawable.ic_action_cancel);

        mMenuLayout = menuLayout;
        mStatusView = statusView;
        mUpButton = (ImageButton) menuLayout.findViewById(R.id.up);
        mUpButton.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    mFileBrowserDialogFragment.simulateBack();
                }
            }
        );

        mAllButton = (ImageButton) menuLayout.findViewById(R.id.all);
        mAllButton.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    toggleAllSelected();
                }
            }
        );

        mAddButton = (ImageButton) menuLayout.findViewById(R.id.add);
        mAddButton.setOnClickListener(this);

        mRecursiveCheckBox = (CheckBox) menuLayout.findViewById(R.id.recursive);
        mRecursiveTextView = (TextView) menuLayout.findViewById(R.id.recursive_text);
        menuLayout.setVisibility(View.GONE);
        statusView.setVisibility(View.GONE);
        statusView.setText("");
    }

    @Override   // View.OnClickListener.onClick for mAddButton
    public void onClick(View v) {
        if (!mSelectionMode)
            return;

//      String[] zipFiles = mFileBrowserDialogFragment.zipFiles();
//      Log.i(TAG, "zipFiles: # = " + zipFiles.length);
//      for (String s: zipFiles)
//          Log.i(TAG, "    |" + s + "|");
        //Log.i(TAG, "# of items = " + checkedStates.size());
        Log.i(TAG, "selected files:...");
        final List<Item> itemList = new ArrayList<Item>();
        for (int i = 0; i < mCheckedStates.size(); ++i) {
            int pos = mCheckedStates.keyAt(i);
            //Log.i(TAG, "items[" + pos + "]: checked? " + checkedStates.get(pos));
            if (!mCheckedStates.get(pos))
                continue;
            EntryEx item = (EntryEx) getItem(pos);
            String name0 = item.getName();
            int type = item.getType();
            Log.i(TAG, "    |" + name0 + "|: " + (type == EntryEx.TYPE_DIR ? "DIR" : (type == EntryEx.TYPE_ZIP ? "ZIP" : "MP3")));
            itemList.add(new Item(type, name0));
        }

/*
        new AudioListSet(
            mActivity,
            itemList,
            zipFiles,
            mFileBrowserDialogFragment.fileFilter(),
            mRecursiveCheckBox.isChecked()
        ).execute();
 */
        (new AsyncTask<Void, Void, List<FileElement>>() {
            @Override
            protected List<FileElement> doInBackground(Void...none)
            {
                String[] zipFiles = mFileBrowserDialogFragment.zipFiles();
                ZipFileExFileFilter fileFilter = mFileBrowserDialogFragment.fileFilter();
                boolean recursive = mRecursiveCheckBox.isChecked();
                String[] selectedList = new String[itemList.size()];
                for (int i = 0; i < itemList.size(); ++i) {
                    Item item = itemList.get(i);
                    Log.i(TAG, "    |" + item.mPathname + "|: " + (item.mType == EntryEx.TYPE_DIR ? "DIR" : (item.mType == EntryEx.TYPE_ZIP ? "ZIP" : "MP3")));
                    selectedList[i] = item.mPathname;
                }
                boolean trace = false;
                List<FileElement> audioFiles = new ArrayList<FileElement>();
                ZipFileExIteration iteration = new ZipFileExIteration(LoadActivity.CONTAINER, fileFilter);
                try {
                    if (zipFiles.length > 0) {
                        iteration.addZipStackElement(fileFilter.getInputStream(zipFiles[0]), zipFiles, selectedList, trace ? 0 : null);
                    } else {
                        iteration.addDirStackElement(
                            selectedList,
                            null,   // not from zip file
                            trace ? 0 : null
                        );
                    }
                    iteration.execStackElement(recursive, audioFiles);
                    for (FileElement fe: audioFiles) {
                        if (fe.zipFiles != null && fe.zipFiles.length > 0 && fe.tmpPathname == null && false) {
                            String copiedStr = null;
                            try {
                                copiedStr = LoadActivity.CONTAINER.copy(fileFilter.getInputStream(fe.zipFiles[0]), fe.zipFiles, fe.pathname, null);
                            } catch (IOException e) {   // including ZipException, FileNotFoundException
                                e.printStackTrace();
                            }
                            if (true) {
                                System.out.print("|" + fe.pathname + "| -> ");
                                System.out.println(copiedStr == null ? "failed" : ("|" + copiedStr + "|"));
                            }
                            fe.tmpPathname = copiedStr;
                        } else {
                            //Log.i(TAG, "|" + fe.pathname + "|");
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                } finally {
                    if (iteration != null)
                        iteration.clear();
                }
                return audioFiles;
            }

            @Override
            protected void onPostExecute(List<FileElement> audioFiles)
            {
                mFileBrowserDialogFragment.done(audioFiles);
            }
        }).execute();

        //mActivity.finish();
    }
    public void setData(List<EntryEx> entryExList) {
        clear();
        if (entryExList != null) {
            addAll(entryExList);
        }
    }

    @Override
    public View getView(final int position, View convertView, final ViewGroup parent) {
        // A ViewHolder keeps references to children views to avoid unneccessary calls
        // to findViewById() on each row.
        ViewHolder holder;

        // When convertView is not null, we can reuse it directly, there is no need
        // to reinflate it. We only inflate a new View when the convertView supplied
        // by ListView is null.
        if (convertView == null) {
            convertView = mInflater.inflate(R.layout.list_item_icon_text_checkbox, null);

            // Creates a ViewHolder and store references to the two children views
            // we want to bind data to.
            holder = new ViewHolder();
            holder.icon = (ImageView) convertView.findViewById(R.id.icon);
            holder.text = (TextView) convertView.findViewById(R.id.text);
            holder.info = (TextView) convertView.findViewById(R.id.info);
            holder.sel = (CheckBox) convertView.findViewById(R.id.select);
            convertView.setTag(holder);
        } else {
            // Get the ViewHolder back to get fast access to the TextView
            // and the ImageView.
            holder = (ViewHolder) convertView.getTag();
        }

        holder.sel.setTag(position);
        holder.sel.setOnCheckedChangeListener(this);

        EntryEx item = getItem(position);
        int type = item.getType();

        holder.icon.setImageBitmap(type == EntryEx.TYPE_DIR ? mFolderIcon : (type == EntryEx.TYPE_ZIP ? mZipIcon : mMp3Icon));

        String displayName = item.getDisplayName();
        if (type == EntryEx.TYPE_MP3)
            holder.info.setVisibility(View.GONE);
        else {
            if (false) {    // DEBUG
                holder.info.setVisibility(View.VISIBLE);
                holder.info.setText(String.valueOf(item.getN()));
            } else {
                holder.info.setVisibility(View.GONE);
                //%%int n = item.getN();
                //%%String nStr = String.valueOf(n);
                //%%displayName += " (" + ((type == EntryEx.TYPE_ZIP && n < 0) ? "X" : nStr) + ")";
            }
        }
        holder.text.setText(displayName);

        holder.sel.setVisibility(mSelectionMode ? View.VISIBLE : View.GONE);
        if (mSelectionMode) {
            boolean isSelectable = item.isSelectable();
            holder.sel.setEnabled(isSelectable);
            if (isSelectable) {
                holder.sel.setChecked(mCheckedStates.get(position));
            }
        }

        return convertView;
    }

    public void setChecked(int position, boolean isChecked)
    {
        mCheckedStates.put(position, isChecked);
        notifyDataSetChanged();
    }
    public boolean isChecked(int position)
    {
        return mCheckedStates.get(position, false);
    }
    // called at ListFragment.onListItemClick
    public void toggle(int position)
    {
        setChecked(position, !isChecked(position));
    }
    // called at CheckBox itself
    @Override   // CompoundButton.OnCheckedChangeListener
    public void onCheckedChanged(CompoundButton checkBoxView, boolean isChecked)
    {
        mCheckedStates.put((Integer) checkBoxView.getTag(), isChecked);

        int numChecked = 0;
        int numMp3 = 0;
        for (int i = 0; i < mCheckedStates.size(); ++i) {
            int pos = mCheckedStates.keyAt(i);
            //Log.i(TAG, "items[" + pos + "]: checked? " + checkedStates.get(pos));
            if (mCheckedStates.get(pos)) {
                ++numChecked;
                EntryEx item = getItem(pos);
                if (item.getType() == EntryEx.TYPE_MP3)
                    ++numMp3;
            }
        }
        mStatusView.setText("" + numChecked + " items selected");
        mAddButton.setVisibility(numChecked > 0 ? View.VISIBLE : View.GONE);
        boolean isDirOrZipSelected = numChecked > 0 && numChecked != numMp3;
        mRecursiveCheckBox.setVisibility(isDirOrZipSelected ? View.VISIBLE : View.GONE);
        mRecursiveTextView.setVisibility(isDirOrZipSelected ? View.VISIBLE : View.GONE);
    }

    static class ViewHolder {
        ImageView icon;
        TextView text;
        TextView info;
        CheckBox sel;
    }
}
