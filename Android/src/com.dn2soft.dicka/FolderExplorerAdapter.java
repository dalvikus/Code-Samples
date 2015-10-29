package com.dn2soft.dicka;
import com.dn2soft.dicka.FolderExplorer.Folder;
import com.dn2soft.dicka.FolderExplorer.Folder.FolderFlatten;

import android.widget.ArrayAdapter;
import android.view.ViewGroup;
import android.widget.TextView;

import android.view.LayoutInflater;
import java.util.List;

import android.content.Context;
import android.app.Activity;

import android.util.Log;

import android.view.View;

public class FolderExplorerAdapter extends ArrayAdapter<FolderFlatten> {
    private static final String TAG = "FolderExplorerAdapter";

    private final LayoutInflater mInflater;
    private final Context mContext;
    public FolderExplorerAdapter(Context context)
    {
        super(context, android.R.layout.simple_list_item_2);
        mContext = context;
        mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    public void setData(List<FolderFlatten> ffal) {
        clear();
        if (ffal != null) {
    Folder currentFolder = LoadActivity.FOLDER_EXPLORER.getCurrentFolder();
    boolean filenameOnly = false;
    for (Folder.FolderFlatten ff: ffal) {
        Folder.printFolder(ff.folder, ff.folder.idStr.equals(currentFolder.idStr), filenameOnly, ff.depth, "....");
        filenameOnly = true;
    }
            addAll(ffal);
        }
    }

    static class ViewHolder {
        TextView indent;
        TextView icon;
        TextView name;
    }
    private static int k = 0;
    @Override
    public View getView(final int position, View convertView, final ViewGroup parent) {
        ViewHolder holder;

        if (convertView == null) {
            convertView = mInflater.inflate(R.layout.list_three_textview, null);
            holder = new ViewHolder();
            holder.indent = (TextView) convertView.findViewById(R.id.indent);
            holder.icon = (TextView) convertView.findViewById(R.id.icon);
            holder.name = (TextView) convertView.findViewById(R.id.name);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }

        FolderFlatten ff = getItem(position);
        int iconType = ff.folder.getIconType();
        String iconStr = Folder.ICON[iconType];
        String displayPathname = ff.folder.getDisplayPathname();
//Log.i(TAG, "" + position + ", " + displayPathname);
        String displayFilename = null;
        if (position > 0)
            displayFilename = displayPathname.substring(1 + displayPathname.lastIndexOf("/"));
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < ff.depth; ++i)
            sb.append("----");
        holder.indent.setText(sb.toString());
        holder.icon.setText(LoadActivity.FOLDER_EXPLORER.getCurrentFolder().idStr.equals(ff.folder.idStr) ? ("<" + iconStr + ">") : iconStr);
        holder.name.setText(position == 0 ? displayPathname : displayFilename);

        return convertView;
    }
}
