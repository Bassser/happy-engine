﻿using System.IO;
using System.Windows.Input;
using DaeMvvmFramework;
using HappyFxEditorContextLib.Effect.ComponentEditor;
using HappyFxEditorContextLib.Effect.TimeLine;

namespace HappyFxEditorContextLib.Effect
{
    public sealed class EffectContext : UndoableContext
    {
        private string _name;
        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }
        public string DisplayName
        {
            get { return Modified ? _name + "*" : _name; }
        }

        public string File { get; set; }

        private bool _modified = false;
        public bool Modified 
        {
            get { return _modified; }
            set { Change(ref _modified, value, "Modified", "DisplayName"); }
        }

        public ICommand CloseCommand { get; private set; }

        public MainContext MainContext { get; private set; }
        public TimeLineContext TimeLine { get; private set; }
        public ComponentEditorContext ComponentEditor { get; private set; }

        public EffectContext(MainContext mainContext, string path)
            : base(new Evolution(100))
        {
            File = path;
            Name = Path.GetFileNameWithoutExtension(path);
            MainContext = mainContext;
            Modified = false;
            CloseCommand = CommandFactory.Create(() => MainContext.Close(this));
            TimeLine = new TimeLineContext(this);
            ComponentEditor = new ComponentEditorContext(this);
            Evolution.Changed += (s, e) => { Modified = true; };
        }
        public EffectContext(MainContext mainContext)
            : this(mainContext, "")
        {
            Name = "NewEffect";
        }

        public void Save()
        {
            Modified = false;
        }
        public void Save(string path)
        {
            File = path;
            Name = Path.GetFileNameWithoutExtension(path);
            Modified = false;
        }
    }
}