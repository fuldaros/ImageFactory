package ru.fuldaros.imgf.util.loader;

import android.os.AsyncTask;
import android.widget.TextView;

import java.io.IOException;

import ru.fuldaros.imgf.core.ImageFactory;
import ru.fuldaros.imgf.util.FileUtils;

/**
 * Created by fuldaros on 2016/8/14.
 */

public class ChangeLogLoader extends AsyncTask<Void, Void, StringBuilder> {
    private TextView textView;

    public ChangeLogLoader(TextView textView) {
        this.textView = textView;
    }

    @Override
    protected void onPostExecute(StringBuilder stringBuilder) {
        super.onPostExecute(stringBuilder);
        textView.setText(stringBuilder);
    }

    @Override
    protected StringBuilder doInBackground(Void... params) {
        StringBuilder sb = new StringBuilder();
        try {
            sb.append(FileUtils.readInputStream(ImageFactory.getApp().getAssets().open("changelog.txt")));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        return sb;
    }
}
