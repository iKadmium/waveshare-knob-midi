export const ATTR_TYPE_CC = 0;
export const ATTR_TYPE_PROGRAM_CHANGE = 1;

interface BaseMidiAttribute {
    attributeType: number;
    title: string;
    channel: number;
}

export interface CcAttribute extends BaseMidiAttribute {
    attributeType: typeof ATTR_TYPE_CC;
    
    cc: number;
}

export interface ProgramChangeAttribute extends BaseMidiAttribute {
    attributeType: typeof ATTR_TYPE_PROGRAM_CHANGE;
    
    programs: string[];
}

export type MidiAttribute = CcAttribute | ProgramChangeAttribute;