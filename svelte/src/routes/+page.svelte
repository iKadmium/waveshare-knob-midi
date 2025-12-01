<script lang="ts">
	import CcAttributeEditor from '$lib/components/CcAttributeEditor.svelte';
	import ProgramChangeAttributeEditor from '$lib/components/ProgramChangeAttributeEditor.svelte';
	import {
		ATTR_TYPE_CC,
		ATTR_TYPE_PROGRAM_CHANGE,
		type ProgramChangeAttribute,
		type CcAttribute
	} from '$lib/models/midi-attribute';
	import type { PageData } from './$types';

	let { data }: { data: PageData } = $props();

	let attributes = $state(data.attributes);
</script>

<main class="container">
	<h1>Midi BLE Knob Settings</h1>

	{#if attributes.length === 0}
		<article>
			<p>No MIDI attributes found. Check your backend connection.</p>
		</article>
	{:else}
		{#each attributes as attribute, i (attribute.title)}
			{#if attribute.attributeType === ATTR_TYPE_CC}
				<CcAttributeEditor
					bind:attribute={attributes[i] as CcAttribute}
					onRemove={() => {
						attributes = attributes.filter((attr) => attr !== attribute);
					}}
				/>
			{:else if attribute.attributeType === ATTR_TYPE_PROGRAM_CHANGE}
				<ProgramChangeAttributeEditor
					bind:attribute={attributes[i] as ProgramChangeAttribute}
					onRemove={() => {
						attributes = attributes.filter((attr) => attr !== attribute);
					}}
				/>
			{/if}
		{/each}
		<div role="group" aria-label="Add Attribute Buttons">
			<button
				onclick={() => {
					attributes = [
						...attributes,
						{
							attributeType: ATTR_TYPE_CC,
							title: 'New CC Attribute',
							channel: 1,
							cc: 0
						}
					];
				}}>Add CC Attribute</button
			>
			<button
				onclick={() => {
					attributes = [
						...attributes,
						{
							attributeType: ATTR_TYPE_PROGRAM_CHANGE,
							title: 'New Program Change Attribute',
							channel: 1,
							programs: []
						}
					];
				}}>Add Program Change Attribute</button
			>
		</div>

		<button
			type="submit"
			class="primary"
			onclick={() => {
				// Here you would typically send the updated attributes to your backend
				console.log('Saving attributes:', JSON.stringify(attributes));
			}}>Save Changes</button
		>
	{/if}
</main>
