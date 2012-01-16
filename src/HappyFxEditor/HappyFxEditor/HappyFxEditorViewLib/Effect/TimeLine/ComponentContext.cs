﻿using System.Windows.Input;
using System.Windows.Media;
using HappyFxEditorBaseLib;
using DaeMvvmFramework;
using HappyFxEditorContextLib.Effect.ComponentEditor;

namespace HappyFxEditorContextLib.Effect.TimeLine
{
    public class ComponentContext : PropertyChangeSource
    {
        public ICommand CutCommand { get; private set; }
        public ICommand CopyCommand { get; private set; }
        public ICommand DeleteCommand { get; private set; }

        public TimeLineTrackContext Track { get; private set; }

        #region Behaviours
        private UndoableCollection<BehaviourContext> _behaviours;
        public const string BehavioursProperty = "Behaviours";
        public UndoableCollection<BehaviourContext> Behaviours
        {
            get { return _behaviours; }
            set { Change(ref _behaviours, value, BehavioursProperty); }
        }
        #endregion       

        public ComponentContext(TimeLineTrackContext track, TimeLineTrackComponentType type)
        {
            Track = track;

            if (Track != null)
                _behaviours = new UndoableCollection<BehaviourContext>(Track.TimeLine.Effect);

            CutCommand = CommandFactory.Create(() =>
                                                   {
                                                       track.TimeLine.ClipBoard = this;
                                                       track.Remove(this);
                                                   });
            CopyCommand = CommandFactory.Create(() => track.TimeLine.ClipBoard = Copy());
            DeleteCommand = CommandFactory.Create(() => track.Remove(this));


            Type = type;
            UpdateColors();
            Hue = (short)((360.0 / ((int)TimeLineTrackComponentType.MAX_TYPES+1)) * (int)type);
            switch (type)
            {
                case TimeLineTrackComponentType.ParticleSystem:
                    Text = "Particle System";
                    break;
                case TimeLineTrackComponentType.CameraFx:
                    Text = "Camera Fx";
                    break;
                case TimeLineTrackComponentType.AudioFx:
                    Text = "Audio Fx";
                    break;
                case TimeLineTrackComponentType.MAX_TYPES:
                    Text = "Select";
                    break;
                default:
                    Text = "";
                    break;
            }
        }

        private int _x = 0;
        public int X 
        {
            get { return _x; }
            set { Change(ref _x, value, "X"); }
        }

        private int _width = 16;
        public int Width
        {
            get { return _width; }
            set { Change(ref _width, value, "Width", "ForegroundWidth"); }
        }

        private int _height = 16;
        public int Height
        {
            get { return _height; }
            set { Change(ref _height, value, "Height", "ForegroundHeight", "FontSize"); }
        }

        private short _hue = 0;
        public short Hue
        {
            get { return _hue; }
            private set { Change(_hue, value, (newValue) => { _hue = newValue; UpdateColors(); }, "Hue", "BackgroundColor", "ForegroundColor"); }
        }

        public TimeLineTrackComponentType Type { get; private set; }

        private string _text = "";
        public string Text
        {
            get { return _text; }
            set { Change(ref _text, value, "Text"); }
        }

        private bool _isSelected = false;
        public bool IsSelected
        {
            get { return _isSelected; }
            set { Change(_isSelected, value, (newValue) => { _isSelected = newValue; UpdateColors(); }, "IsSelected", "BackgroundColor", "ForegroundColor"); }
        }

        public int ForegroundWidth
        {
            get
            {
                return Width - 8;
            }
        }
        public int ForegroundHeight { get { return Height - 8; } }

        public double FontSize { get { return ForegroundHeight / 1.499998125; } } //Magic!

        public Brush BackgroundColor { get; private set; }
        public Brush ForegroundColor { get; private set; }

        private void UpdateColors()
        {
            BackgroundColor = new SolidColorBrush(ColorExtensions.FromHsb(Hue, 0.89, IsSelected ? 0.80 : 0.37));
            ForegroundColor = new SolidColorBrush(ColorExtensions.FromHsb((short)(Hue + 16), IsSelected ? 0.40 : 0.80, IsSelected ? 0.90 : 0.86));
        }

        internal ComponentContext Copy()
        {
            ComponentContext comp = new ComponentContext(Track, Type);
            comp.Height = Height;
            comp.X = X;
            comp.Width = Width;
            return comp;
        }

        internal void AddBehaviour(BehaviourContext behaviourContext)
        {
            Behaviours.Add(behaviourContext);
        }
    }
}