package ru.fuldaros.imgf.adapter;

import android.support.v7.widget.RecyclerView;
import android.view.ViewGroup;

/**
 * Created by fuldaros on 2017/10/28.
 */
public class BaseRecyclerAdapter extends RecyclerView.Adapter<BaseViewHolder> {

    @Override
    public BaseViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        return null;
    }

    @Override
    public void onBindViewHolder(BaseViewHolder holder, int position) {

    }

    @Override
    public int getItemCount() {
        return 0;
    }
}
