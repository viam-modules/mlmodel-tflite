// Code generated by the FlatBuffers compiler. DO NOT EDIT.

package tflite_metadata

import (
	flatbuffers "github.com/google/flatbuffers/go"
)

type SubGraphMetadataT struct {
	Name                 string
	Description          string
	InputTensorMetadata  []*TensorMetadataT
	OutputTensorMetadata []*TensorMetadataT
	AssociatedFiles      []*AssociatedFileT
	InputProcessUnits    []*ProcessUnitT
	OutputProcessUnits   []*ProcessUnitT
	InputTensorGroups    []*TensorGroupT
	OutputTensorGroups   []*TensorGroupT
}

func (t *SubGraphMetadataT) Pack(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
	if t == nil {
		return 0
	}
	nameOffset := builder.CreateString(t.Name)
	descriptionOffset := builder.CreateString(t.Description)
	inputTensorMetadataOffset := flatbuffers.UOffsetT(0)
	if t.InputTensorMetadata != nil {
		inputTensorMetadataLength := len(t.InputTensorMetadata)
		inputTensorMetadataOffsets := make([]flatbuffers.UOffsetT, inputTensorMetadataLength)
		for j := 0; j < inputTensorMetadataLength; j++ {
			inputTensorMetadataOffsets[j] = t.InputTensorMetadata[j].Pack(builder)
		}
		SubGraphMetadataStartInputTensorMetadataVector(builder, inputTensorMetadataLength)
		for j := inputTensorMetadataLength - 1; j >= 0; j-- {
			builder.PrependUOffsetT(inputTensorMetadataOffsets[j])
		}
		inputTensorMetadataOffset = builder.EndVector(inputTensorMetadataLength)
	}
	outputTensorMetadataOffset := flatbuffers.UOffsetT(0)
	if t.OutputTensorMetadata != nil {
		outputTensorMetadataLength := len(t.OutputTensorMetadata)
		outputTensorMetadataOffsets := make([]flatbuffers.UOffsetT, outputTensorMetadataLength)
		for j := 0; j < outputTensorMetadataLength; j++ {
			outputTensorMetadataOffsets[j] = t.OutputTensorMetadata[j].Pack(builder)
		}
		SubGraphMetadataStartOutputTensorMetadataVector(builder, outputTensorMetadataLength)
		for j := outputTensorMetadataLength - 1; j >= 0; j-- {
			builder.PrependUOffsetT(outputTensorMetadataOffsets[j])
		}
		outputTensorMetadataOffset = builder.EndVector(outputTensorMetadataLength)
	}
	associatedFilesOffset := flatbuffers.UOffsetT(0)
	if t.AssociatedFiles != nil {
		associatedFilesLength := len(t.AssociatedFiles)
		associatedFilesOffsets := make([]flatbuffers.UOffsetT, associatedFilesLength)
		for j := 0; j < associatedFilesLength; j++ {
			associatedFilesOffsets[j] = t.AssociatedFiles[j].Pack(builder)
		}
		SubGraphMetadataStartAssociatedFilesVector(builder, associatedFilesLength)
		for j := associatedFilesLength - 1; j >= 0; j-- {
			builder.PrependUOffsetT(associatedFilesOffsets[j])
		}
		associatedFilesOffset = builder.EndVector(associatedFilesLength)
	}
	inputProcessUnitsOffset := flatbuffers.UOffsetT(0)
	if t.InputProcessUnits != nil {
		inputProcessUnitsLength := len(t.InputProcessUnits)
		inputProcessUnitsOffsets := make([]flatbuffers.UOffsetT, inputProcessUnitsLength)
		for j := 0; j < inputProcessUnitsLength; j++ {
			inputProcessUnitsOffsets[j] = t.InputProcessUnits[j].Pack(builder)
		}
		SubGraphMetadataStartInputProcessUnitsVector(builder, inputProcessUnitsLength)
		for j := inputProcessUnitsLength - 1; j >= 0; j-- {
			builder.PrependUOffsetT(inputProcessUnitsOffsets[j])
		}
		inputProcessUnitsOffset = builder.EndVector(inputProcessUnitsLength)
	}
	outputProcessUnitsOffset := flatbuffers.UOffsetT(0)
	if t.OutputProcessUnits != nil {
		outputProcessUnitsLength := len(t.OutputProcessUnits)
		outputProcessUnitsOffsets := make([]flatbuffers.UOffsetT, outputProcessUnitsLength)
		for j := 0; j < outputProcessUnitsLength; j++ {
			outputProcessUnitsOffsets[j] = t.OutputProcessUnits[j].Pack(builder)
		}
		SubGraphMetadataStartOutputProcessUnitsVector(builder, outputProcessUnitsLength)
		for j := outputProcessUnitsLength - 1; j >= 0; j-- {
			builder.PrependUOffsetT(outputProcessUnitsOffsets[j])
		}
		outputProcessUnitsOffset = builder.EndVector(outputProcessUnitsLength)
	}
	inputTensorGroupsOffset := flatbuffers.UOffsetT(0)
	if t.InputTensorGroups != nil {
		inputTensorGroupsLength := len(t.InputTensorGroups)
		inputTensorGroupsOffsets := make([]flatbuffers.UOffsetT, inputTensorGroupsLength)
		for j := 0; j < inputTensorGroupsLength; j++ {
			inputTensorGroupsOffsets[j] = t.InputTensorGroups[j].Pack(builder)
		}
		SubGraphMetadataStartInputTensorGroupsVector(builder, inputTensorGroupsLength)
		for j := inputTensorGroupsLength - 1; j >= 0; j-- {
			builder.PrependUOffsetT(inputTensorGroupsOffsets[j])
		}
		inputTensorGroupsOffset = builder.EndVector(inputTensorGroupsLength)
	}
	outputTensorGroupsOffset := flatbuffers.UOffsetT(0)
	if t.OutputTensorGroups != nil {
		outputTensorGroupsLength := len(t.OutputTensorGroups)
		outputTensorGroupsOffsets := make([]flatbuffers.UOffsetT, outputTensorGroupsLength)
		for j := 0; j < outputTensorGroupsLength; j++ {
			outputTensorGroupsOffsets[j] = t.OutputTensorGroups[j].Pack(builder)
		}
		SubGraphMetadataStartOutputTensorGroupsVector(builder, outputTensorGroupsLength)
		for j := outputTensorGroupsLength - 1; j >= 0; j-- {
			builder.PrependUOffsetT(outputTensorGroupsOffsets[j])
		}
		outputTensorGroupsOffset = builder.EndVector(outputTensorGroupsLength)
	}
	SubGraphMetadataStart(builder)
	SubGraphMetadataAddName(builder, nameOffset)
	SubGraphMetadataAddDescription(builder, descriptionOffset)
	SubGraphMetadataAddInputTensorMetadata(builder, inputTensorMetadataOffset)
	SubGraphMetadataAddOutputTensorMetadata(builder, outputTensorMetadataOffset)
	SubGraphMetadataAddAssociatedFiles(builder, associatedFilesOffset)
	SubGraphMetadataAddInputProcessUnits(builder, inputProcessUnitsOffset)
	SubGraphMetadataAddOutputProcessUnits(builder, outputProcessUnitsOffset)
	SubGraphMetadataAddInputTensorGroups(builder, inputTensorGroupsOffset)
	SubGraphMetadataAddOutputTensorGroups(builder, outputTensorGroupsOffset)
	return SubGraphMetadataEnd(builder)
}

func (rcv *SubGraphMetadata) UnPackTo(t *SubGraphMetadataT) {
	t.Name = string(rcv.Name())
	t.Description = string(rcv.Description())
	inputTensorMetadataLength := rcv.InputTensorMetadataLength()
	t.InputTensorMetadata = make([]*TensorMetadataT, inputTensorMetadataLength)
	for j := 0; j < inputTensorMetadataLength; j++ {
		x := TensorMetadata{}
		rcv.InputTensorMetadata(&x, j)
		t.InputTensorMetadata[j] = x.UnPack()
	}
	outputTensorMetadataLength := rcv.OutputTensorMetadataLength()
	t.OutputTensorMetadata = make([]*TensorMetadataT, outputTensorMetadataLength)
	for j := 0; j < outputTensorMetadataLength; j++ {
		x := TensorMetadata{}
		rcv.OutputTensorMetadata(&x, j)
		t.OutputTensorMetadata[j] = x.UnPack()
	}
	associatedFilesLength := rcv.AssociatedFilesLength()
	t.AssociatedFiles = make([]*AssociatedFileT, associatedFilesLength)
	for j := 0; j < associatedFilesLength; j++ {
		x := AssociatedFile{}
		rcv.AssociatedFiles(&x, j)
		t.AssociatedFiles[j] = x.UnPack()
	}
	inputProcessUnitsLength := rcv.InputProcessUnitsLength()
	t.InputProcessUnits = make([]*ProcessUnitT, inputProcessUnitsLength)
	for j := 0; j < inputProcessUnitsLength; j++ {
		x := ProcessUnit{}
		rcv.InputProcessUnits(&x, j)
		t.InputProcessUnits[j] = x.UnPack()
	}
	outputProcessUnitsLength := rcv.OutputProcessUnitsLength()
	t.OutputProcessUnits = make([]*ProcessUnitT, outputProcessUnitsLength)
	for j := 0; j < outputProcessUnitsLength; j++ {
		x := ProcessUnit{}
		rcv.OutputProcessUnits(&x, j)
		t.OutputProcessUnits[j] = x.UnPack()
	}
	inputTensorGroupsLength := rcv.InputTensorGroupsLength()
	t.InputTensorGroups = make([]*TensorGroupT, inputTensorGroupsLength)
	for j := 0; j < inputTensorGroupsLength; j++ {
		x := TensorGroup{}
		rcv.InputTensorGroups(&x, j)
		t.InputTensorGroups[j] = x.UnPack()
	}
	outputTensorGroupsLength := rcv.OutputTensorGroupsLength()
	t.OutputTensorGroups = make([]*TensorGroupT, outputTensorGroupsLength)
	for j := 0; j < outputTensorGroupsLength; j++ {
		x := TensorGroup{}
		rcv.OutputTensorGroups(&x, j)
		t.OutputTensorGroups[j] = x.UnPack()
	}
}

func (rcv *SubGraphMetadata) UnPack() *SubGraphMetadataT {
	if rcv == nil {
		return nil
	}
	t := &SubGraphMetadataT{}
	rcv.UnPackTo(t)
	return t
}

type SubGraphMetadata struct {
	_tab flatbuffers.Table
}

func GetRootAsSubGraphMetadata(buf []byte, offset flatbuffers.UOffsetT) *SubGraphMetadata {
	n := flatbuffers.GetUOffsetT(buf[offset:])
	x := &SubGraphMetadata{}
	x.Init(buf, n+offset)
	return x
}

func GetSizePrefixedRootAsSubGraphMetadata(buf []byte, offset flatbuffers.UOffsetT) *SubGraphMetadata {
	n := flatbuffers.GetUOffsetT(buf[offset+flatbuffers.SizeUint32:])
	x := &SubGraphMetadata{}
	x.Init(buf, n+offset+flatbuffers.SizeUint32)
	return x
}

func (rcv *SubGraphMetadata) Init(buf []byte, i flatbuffers.UOffsetT) {
	rcv._tab.Bytes = buf
	rcv._tab.Pos = i
}

func (rcv *SubGraphMetadata) Table() flatbuffers.Table {
	return rcv._tab
}

func (rcv *SubGraphMetadata) Name() []byte {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(4))
	if o != 0 {
		return rcv._tab.ByteVector(o + rcv._tab.Pos)
	}
	return nil
}

func (rcv *SubGraphMetadata) Description() []byte {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(6))
	if o != 0 {
		return rcv._tab.ByteVector(o + rcv._tab.Pos)
	}
	return nil
}

func (rcv *SubGraphMetadata) InputTensorMetadata(obj *TensorMetadata, j int) bool {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(8))
	if o != 0 {
		x := rcv._tab.Vector(o)
		x += flatbuffers.UOffsetT(j) * 4
		x = rcv._tab.Indirect(x)
		obj.Init(rcv._tab.Bytes, x)
		return true
	}
	return false
}

func (rcv *SubGraphMetadata) InputTensorMetadataLength() int {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(8))
	if o != 0 {
		return rcv._tab.VectorLen(o)
	}
	return 0
}

func (rcv *SubGraphMetadata) OutputTensorMetadata(obj *TensorMetadata, j int) bool {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(10))
	if o != 0 {
		x := rcv._tab.Vector(o)
		x += flatbuffers.UOffsetT(j) * 4
		x = rcv._tab.Indirect(x)
		obj.Init(rcv._tab.Bytes, x)
		return true
	}
	return false
}

func (rcv *SubGraphMetadata) OutputTensorMetadataLength() int {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(10))
	if o != 0 {
		return rcv._tab.VectorLen(o)
	}
	return 0
}

func (rcv *SubGraphMetadata) AssociatedFiles(obj *AssociatedFile, j int) bool {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(12))
	if o != 0 {
		x := rcv._tab.Vector(o)
		x += flatbuffers.UOffsetT(j) * 4
		x = rcv._tab.Indirect(x)
		obj.Init(rcv._tab.Bytes, x)
		return true
	}
	return false
}

func (rcv *SubGraphMetadata) AssociatedFilesLength() int {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(12))
	if o != 0 {
		return rcv._tab.VectorLen(o)
	}
	return 0
}

func (rcv *SubGraphMetadata) InputProcessUnits(obj *ProcessUnit, j int) bool {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(14))
	if o != 0 {
		x := rcv._tab.Vector(o)
		x += flatbuffers.UOffsetT(j) * 4
		x = rcv._tab.Indirect(x)
		obj.Init(rcv._tab.Bytes, x)
		return true
	}
	return false
}

func (rcv *SubGraphMetadata) InputProcessUnitsLength() int {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(14))
	if o != 0 {
		return rcv._tab.VectorLen(o)
	}
	return 0
}

func (rcv *SubGraphMetadata) OutputProcessUnits(obj *ProcessUnit, j int) bool {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(16))
	if o != 0 {
		x := rcv._tab.Vector(o)
		x += flatbuffers.UOffsetT(j) * 4
		x = rcv._tab.Indirect(x)
		obj.Init(rcv._tab.Bytes, x)
		return true
	}
	return false
}

func (rcv *SubGraphMetadata) OutputProcessUnitsLength() int {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(16))
	if o != 0 {
		return rcv._tab.VectorLen(o)
	}
	return 0
}

func (rcv *SubGraphMetadata) InputTensorGroups(obj *TensorGroup, j int) bool {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(18))
	if o != 0 {
		x := rcv._tab.Vector(o)
		x += flatbuffers.UOffsetT(j) * 4
		x = rcv._tab.Indirect(x)
		obj.Init(rcv._tab.Bytes, x)
		return true
	}
	return false
}

func (rcv *SubGraphMetadata) InputTensorGroupsLength() int {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(18))
	if o != 0 {
		return rcv._tab.VectorLen(o)
	}
	return 0
}

func (rcv *SubGraphMetadata) OutputTensorGroups(obj *TensorGroup, j int) bool {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(20))
	if o != 0 {
		x := rcv._tab.Vector(o)
		x += flatbuffers.UOffsetT(j) * 4
		x = rcv._tab.Indirect(x)
		obj.Init(rcv._tab.Bytes, x)
		return true
	}
	return false
}

func (rcv *SubGraphMetadata) OutputTensorGroupsLength() int {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(20))
	if o != 0 {
		return rcv._tab.VectorLen(o)
	}
	return 0
}

func SubGraphMetadataStart(builder *flatbuffers.Builder) {
	builder.StartObject(9)
}
func SubGraphMetadataAddName(builder *flatbuffers.Builder, name flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(0, flatbuffers.UOffsetT(name), 0)
}
func SubGraphMetadataAddDescription(builder *flatbuffers.Builder, description flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(1, flatbuffers.UOffsetT(description), 0)
}
func SubGraphMetadataAddInputTensorMetadata(builder *flatbuffers.Builder, inputTensorMetadata flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(2, flatbuffers.UOffsetT(inputTensorMetadata), 0)
}
func SubGraphMetadataStartInputTensorMetadataVector(builder *flatbuffers.Builder, numElems int) flatbuffers.UOffsetT {
	return builder.StartVector(4, numElems, 4)
}
func SubGraphMetadataAddOutputTensorMetadata(builder *flatbuffers.Builder, outputTensorMetadata flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(3, flatbuffers.UOffsetT(outputTensorMetadata), 0)
}
func SubGraphMetadataStartOutputTensorMetadataVector(builder *flatbuffers.Builder, numElems int) flatbuffers.UOffsetT {
	return builder.StartVector(4, numElems, 4)
}
func SubGraphMetadataAddAssociatedFiles(builder *flatbuffers.Builder, associatedFiles flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(4, flatbuffers.UOffsetT(associatedFiles), 0)
}
func SubGraphMetadataStartAssociatedFilesVector(builder *flatbuffers.Builder, numElems int) flatbuffers.UOffsetT {
	return builder.StartVector(4, numElems, 4)
}
func SubGraphMetadataAddInputProcessUnits(builder *flatbuffers.Builder, inputProcessUnits flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(5, flatbuffers.UOffsetT(inputProcessUnits), 0)
}
func SubGraphMetadataStartInputProcessUnitsVector(builder *flatbuffers.Builder, numElems int) flatbuffers.UOffsetT {
	return builder.StartVector(4, numElems, 4)
}
func SubGraphMetadataAddOutputProcessUnits(builder *flatbuffers.Builder, outputProcessUnits flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(6, flatbuffers.UOffsetT(outputProcessUnits), 0)
}
func SubGraphMetadataStartOutputProcessUnitsVector(builder *flatbuffers.Builder, numElems int) flatbuffers.UOffsetT {
	return builder.StartVector(4, numElems, 4)
}
func SubGraphMetadataAddInputTensorGroups(builder *flatbuffers.Builder, inputTensorGroups flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(7, flatbuffers.UOffsetT(inputTensorGroups), 0)
}
func SubGraphMetadataStartInputTensorGroupsVector(builder *flatbuffers.Builder, numElems int) flatbuffers.UOffsetT {
	return builder.StartVector(4, numElems, 4)
}
func SubGraphMetadataAddOutputTensorGroups(builder *flatbuffers.Builder, outputTensorGroups flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(8, flatbuffers.UOffsetT(outputTensorGroups), 0)
}
func SubGraphMetadataStartOutputTensorGroupsVector(builder *flatbuffers.Builder, numElems int) flatbuffers.UOffsetT {
	return builder.StartVector(4, numElems, 4)
}
func SubGraphMetadataEnd(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
	return builder.EndObject()
}
