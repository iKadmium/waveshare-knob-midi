import {
	ATTR_TYPE_CC,
	ATTR_TYPE_PROGRAM_CHANGE,
	type MidiAttribute
} from '$lib/models/midi-attribute';
import type { PageLoad } from './$types';

export const ssr = false;

const mockData: MidiAttribute[] = [
	{
		attributeType: ATTR_TYPE_CC,
		title: 'DAW Volume',
		channel: 1,
		cc: 7
	},
	{
		attributeType: ATTR_TYPE_CC,
		title: 'Mic Volume',
		channel: 1,
		cc: 10
	},
	{
		attributeType: ATTR_TYPE_CC,
		title: 'Guitar Volume',
		channel: 1,
		cc: 11
	},
	{
		attributeType: ATTR_TYPE_PROGRAM_CHANGE,
		title: 'Guitar Effects',
		channel: 1,
		programs: ['Clean', 'Crunch', 'Rhythm', 'Lead']
	}
];

const enableMock = true;

export const load: PageLoad = async ({ fetch }) => {
	try {
		// Fetch MIDI attributes from the backend endpoint
		// Adjust the URL to match your backend API endpoint
		if (enableMock) {
			return {
				attributes: mockData
			};
		} else {
			const response = await fetch('/api/attributes');

			if (!response.ok) {
				throw new Error(`Failed to fetch attributes: ${response.statusText}`);
			}

			const attributes: MidiAttribute[] = await response.json();

			return {
				attributes
			};
		}
	} catch (error) {
		console.error('Error loading MIDI attributes:', error);
		// Return empty array if fetch fails
		return {
			attributes: [] as MidiAttribute[]
		};
	}
};
