﻿using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using HappyFxEditorContextLib.Effect.TimeLine;

namespace HappyFxEditorApp.Effect.TimeLine
{
    /// <summary>
    /// Interaction logic for TimeLineTrackContainer.xaml
    /// </summary>
    public partial class TimeLineTrackContainer : UserControl
    {
        public TimeLineTrackContainer()
        {
            InitializeComponent();
        }

        public double HorizontalScroll
        {
            get { return _scrollviewer.HorizontalOffset; }
            set { _scrollviewer.ScrollToHorizontalOffset(value); }
        }

        public TimeLineTrackContext SelectedTrack
        {
            get { return (TimeLineTrackContext)GetValue(SelectedTrackProperty); }
            set { SetValue(SelectedTrackProperty, value); }
        }
        public static readonly DependencyProperty SelectedTrackProperty =
            DependencyProperty.Register("SelectedTrack", typeof(TimeLineTrackContext), typeof(TimeLineTrackContainer), new UIPropertyMetadata(null));

        private TimeLineTrackView _selectedTrackView;

        private void TrackMouseDown(object sender, MouseButtonEventArgs e)
        {
            TimeLineTrackView send = (TimeLineTrackView)sender;
            SelectedTrack = (TimeLineTrackContext)(send.DataContext);
            _selectedTrackView = send;
            send.CaptureMouse();
            send.Focus();
            SelectedTrack.OnMouseDown((int)e.GetPosition(send).X);
        }
        private void TrackMouseUp(object sender, MouseButtonEventArgs e)
        {
            TimeLineTrackView send = (TimeLineTrackView)sender;
            send.ReleaseMouseCapture();
            ((TimeLineTrackContext)(send.DataContext)).OnMouseUp((int)e.GetPosition(send).X);
        }
        private void TrackMouseMove(object sender, MouseEventArgs e)
        {
            ((TimeLineTrackContext)(((TimeLineTrackView)sender).DataContext)).OnMouseMove((int)e.GetPosition((TimeLineTrackView)sender).X);
        }

        protected override void OnPreviewKeyDown(KeyEventArgs e)
        {
            if (e.Key == Key.Space)
                SelectedTrack.OnCreateMoveModifierPressed();
            base.OnPreviewKeyDown(e);
        }
        protected override void OnPreviewKeyUp(KeyEventArgs e)
        {
            if (e.Key == Key.Space)
                SelectedTrack.OnCreateMoveModifierReleased((int)Mouse.GetPosition(_selectedTrackView).X);
            base.OnPreviewKeyDown(e);
        }

        private void TrackPreviewMouseUp(object sender, MouseButtonEventArgs e)
        {
            ((TimeLineTrackContext)(((TimeLineTrackView)sender).DataContext)).ContextClickPos = (int)e.GetPosition((TimeLineTrackView)sender).X;
        }
    }
}