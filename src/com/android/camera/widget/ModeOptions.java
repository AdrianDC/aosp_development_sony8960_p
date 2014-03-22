/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.camera.widget;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Canvas;
import android.graphics.Path;
import android.graphics.Paint;
import android.graphics.RectF;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.util.AttributeSet;

import com.android.camera.ui.TopRightWeightedLayout;
import com.android.camera.util.Gusterpolator;
import com.android.camera2.R;

import java.util.ArrayList;

public class ModeOptions extends FrameLayout {
    private int mBackgroundColor;
    private final Paint mPaint = new Paint();
    private boolean mIsHiddenOrHiding;
    private RectF mAnimateFrom = new RectF();
    private View mViewToShowHide;
    private TopRightWeightedLayout mModeOptionsButtons;

    private AnimatorSet mVisibleAnimator;
    private AnimatorSet mHiddenAnimator;
    private boolean mDrawCircle;
    private boolean mFill;
    private static final int RADIUS_ANIMATION_TIME = 250;
    private static final int SHOW_ALPHA_ANIMATION_TIME = 350;
    private static final int HIDE_ALPHA_ANIMATION_TIME = 200;
    private static final int PADDING_ANIMATION_TIME = 350;

    private int mParentSize;
    private boolean mIsPortrait;
    private float mRadius = 0f;

    public ModeOptions(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void setViewToShowHide(View v) {
        mViewToShowHide = v;
    }

    @Override
    public void onFinishInflate() {
        mIsHiddenOrHiding = true;
        mBackgroundColor = getResources().getColor(R.color.mode_options_background);
        mPaint.setAntiAlias(true);
        mPaint.setColor(mBackgroundColor);
        mModeOptionsButtons = (TopRightWeightedLayout) findViewById(R.id.mode_options_buttons);
    }

    @Override
    public void onConfigurationChanged(Configuration config) {
        super.onConfigurationChanged(config);

        FrameLayout.LayoutParams params =
            (FrameLayout.LayoutParams) mModeOptionsButtons.getLayoutParams();

        if (config.orientation == Configuration.ORIENTATION_PORTRAIT) {
            params.gravity = Gravity.RIGHT;
        } else {
            params.gravity = Gravity.TOP;
        }

        mModeOptionsButtons.setLayoutParams(params);
    }


    @Override
    public void onLayout(boolean changed, int left, int top, int right, int bottom) {
        if (changed) {
            mIsPortrait = (getResources().getConfiguration().orientation ==
                           Configuration.ORIENTATION_PORTRAIT);

            int buttonSize = getResources()
                .getDimensionPixelSize(R.dimen.option_button_circle_size);
            int buttonPadding = getResources()
                .getDimensionPixelSize(R.dimen.mode_options_toggle_padding);

            float rLeft, rRight, rTop, rBottom;
            View parent = (View) getParent();
            if (mIsPortrait) {
                rLeft = getWidth() - buttonPadding - buttonSize;
                rTop = (getHeight() - buttonSize) / 2.0f;

                mParentSize = parent.getWidth();
            } else {
                rLeft = buttonPadding;
                rTop = buttonPadding;

                mParentSize = parent.getHeight();
            }
            rRight = rLeft + buttonSize;
            rBottom = rTop + buttonSize;
            mAnimateFrom.set(rLeft, rTop, rRight, rBottom);

            setupAnimators();
        }

        super.onLayout(changed, left, top, right, bottom);
    }

    @Override
    public void onDraw(Canvas canvas) {
        if (mDrawCircle) {
            canvas.drawCircle(mAnimateFrom.centerX(), mAnimateFrom.centerY(), mRadius, mPaint);
        } else if (mFill) {
            canvas.drawPaint(mPaint);
        }
        super.onDraw(canvas);
    }

    private void setupAnimators() {
        if (mVisibleAnimator != null) {
            mVisibleAnimator.end();
        }
        if (mHiddenAnimator != null) {
            mHiddenAnimator.end();
        }

        final float fullSize = (mIsPortrait ? (float) getWidth() : (float) getHeight());

        // show
        {
            final ValueAnimator radiusAnimator =
                ValueAnimator.ofFloat(mAnimateFrom.width()/2.0f,
                    fullSize-mAnimateFrom.width()/2.0f);
            radiusAnimator.setDuration(RADIUS_ANIMATION_TIME);
            radiusAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator animation) {
                    mRadius = (Float) animation.getAnimatedValue();
                    mDrawCircle = true;
                    mFill = false;
                }
            });
            radiusAnimator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    mDrawCircle = false;
                    mFill = true;
                }
            });

            final ValueAnimator alphaAnimator = ValueAnimator.ofFloat(0.0f, 1.0f);
            alphaAnimator.setDuration(SHOW_ALPHA_ANIMATION_TIME);
            alphaAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator animation) {
                    mModeOptionsButtons.setAlpha((Float) animation.getAnimatedValue());
                }
            });
            alphaAnimator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    mModeOptionsButtons.setAlpha(1.0f);
                }
            });

            final int deltaX = getResources()
                .getDimensionPixelSize(R.dimen.mode_options_buttons_anim_delta_x);
            int childCount = ((ViewGroup) mModeOptionsButtons).getChildCount();
            ArrayList<Animator> paddingAnimators = new ArrayList<Animator>();
            for (int i = 0; i < childCount; i++) {
                final View button;
                if (mIsPortrait) {
                    button = ((ViewGroup) mModeOptionsButtons).getChildAt(i);
                } else {
                    button = ((ViewGroup) mModeOptionsButtons).getChildAt(childCount-1-i);
                }

                if (button.getVisibility() == View.VISIBLE) {
                    final ValueAnimator paddingAnimator =
                        ValueAnimator.ofFloat((float) (deltaX*(childCount-i)), 0.0f);
                    paddingAnimator.setDuration(PADDING_ANIMATION_TIME);
                    paddingAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                        @Override
                        public void onAnimationUpdate(ValueAnimator animation) {
                            if (mIsPortrait) {
                                button.setTranslationX((Float) animation.getAnimatedValue());
                            } else {
                                button.setTranslationY(-((Float) animation.getAnimatedValue()));
                            }
                            invalidate();
                        }
                    });

                    paddingAnimators.add(paddingAnimator);
                }
            }
            AnimatorSet paddingAnimatorSet = new AnimatorSet();
            paddingAnimatorSet.playTogether(paddingAnimators);

            mVisibleAnimator = new AnimatorSet();
            mVisibleAnimator.setInterpolator(Gusterpolator.INSTANCE);
            mVisibleAnimator.playTogether(radiusAnimator, alphaAnimator, paddingAnimatorSet);
        }

        // hide
        {
            final ValueAnimator radiusAnimator =
                ValueAnimator.ofFloat(fullSize-mAnimateFrom.width()/2.0f,
                    mAnimateFrom.width()/2.0f);
            radiusAnimator.setDuration(RADIUS_ANIMATION_TIME);
            radiusAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator animation) {
                    mRadius = (Float) animation.getAnimatedValue();
                    mDrawCircle = true;
                    mFill = false;
                    invalidate();
                }
            });
            radiusAnimator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    if (mViewToShowHide != null) {
                        mViewToShowHide.setVisibility(View.VISIBLE);
                        mDrawCircle = false;
                        mFill = false;
                        invalidate();
                    }
                }
            });

            final ValueAnimator alphaAnimator = ValueAnimator.ofFloat(1.0f, 0.0f);
            alphaAnimator.setDuration(HIDE_ALPHA_ANIMATION_TIME);
            alphaAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator animation) {
                    mModeOptionsButtons.setAlpha((Float) animation.getAnimatedValue());
                    invalidate();
                }
            });
            alphaAnimator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    mModeOptionsButtons.setAlpha(0.0f);
                    setVisibility(View.INVISIBLE);
                    invalidate();
                }
            });

            mHiddenAnimator = new AnimatorSet();
            mHiddenAnimator.setInterpolator(Gusterpolator.INSTANCE);
            mHiddenAnimator.playTogether(radiusAnimator, alphaAnimator);
        }
    }

    public void animateVisible() {
        if (mIsHiddenOrHiding) {
            if (mViewToShowHide != null) {
                mViewToShowHide.setVisibility(View.INVISIBLE);
            }
            mHiddenAnimator.cancel();
            mVisibleAnimator.end();
            setVisibility(View.VISIBLE);
            mVisibleAnimator.start();
        }
        mIsHiddenOrHiding = false;
    }

    public void animateHidden() {
        if (!mIsHiddenOrHiding) {
            mVisibleAnimator.cancel();
            mHiddenAnimator.end();
            mHiddenAnimator.start();
        }
        mIsHiddenOrHiding = true;
    }
}