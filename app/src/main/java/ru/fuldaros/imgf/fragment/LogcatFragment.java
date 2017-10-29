package ru.fuldaros.imgf.fragment;

import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.widget.AppCompatTextView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import ru.fuldaros.imgf.R;
import ru.fuldaros.imgf.activity.BaseActivity;
import ru.fuldaros.imgf.core.Debug;

/**
 * Created by fuldaros on 16-8-24.
 */
public class LogcatFragment extends BaseFragment {
    private AppCompatTextView content = null;

    public static BaseFragment newInstance(BaseActivity activity) {
        LogcatFragment fragment = new LogcatFragment();
        fragment.setActivity(activity);
        return fragment;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View root = getContentView();
        if (root == null) {
            root = inflater.inflate(R.layout.fragment_logcat, container, false);
            setContentView(root);
            content = (AppCompatTextView) findViewById(R.id.content);
            content.setVerticalScrollBarEnabled(true);
            new LogcatReader(content).execute();
        }
        return root;
    }

    class LogcatReader extends AsyncTask<Void, Void, StringBuilder> {
        private TextView textView;

        public LogcatReader(TextView textView) {
            this.textView = textView;
        }

        @Override
        protected void onPostExecute(StringBuilder stringBuilder) {
            super.onPostExecute(stringBuilder);
            textView.setText(stringBuilder);
        }

        @Override
        protected StringBuilder doInBackground(Void... params) {
            return Debug.getContent();
        }
    }
}
