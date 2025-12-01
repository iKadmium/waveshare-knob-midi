<script lang="ts">
	import type { ProgramChangeAttribute } from '$lib/models/midi-attribute';

	interface ProgramChangeAttributeEditorProps {
		attribute: ProgramChangeAttribute;
		onRemove?: () => unknown;
	}

	let { attribute = $bindable(), onRemove }: ProgramChangeAttributeEditorProps = $props();

	function handleTitleChange(event: Event) {
		const target = event.target as HTMLInputElement;
		const updated = { ...attribute, title: target.value };
		attribute = updated;
	}

	function handleChannelChange(event: Event) {
		const target = event.target as HTMLInputElement;
		const channel = parseInt(target.value, 10);
		if (!isNaN(channel) && channel >= 1 && channel <= 16) {
			const updated = { ...attribute, channel };
			attribute = updated;
		}
	}

	function handleProgramChange(index: number, event: Event) {
		const target = event.target as HTMLInputElement;
		const programs = [...attribute.programs];
		programs[index] = target.value;
		const updated = { ...attribute, programs };
		attribute = updated;
	}

	function addProgram() {
		const programs = [...attribute.programs, ''];
		const updated = { ...attribute, programs };
		attribute = updated;
	}

	function removeProgram(index: number) {
		const programs = attribute.programs.filter((_, i) => i !== index);
		const updated = { ...attribute, programs };
		attribute = updated;
	}
</script>

<article>
	<header>
		<div class="header-content">
			<h3>Program Change</h3>
			<button type="button" class="secondary" onclick={onRemove}>🚫</button>
		</div>
	</header>

	<form>
		<div class="grid">
			<label>
				Title
				<input
					type="text"
					value={attribute.title}
					oninput={handleTitleChange}
					placeholder="Enter title"
				/>
			</label>

			<label>
				MIDI Channel (1-16)
				<input
					type="number"
					value={attribute.channel}
					oninput={handleChannelChange}
					min="1"
					max="16"
					placeholder="1"
				/>
			</label>
		</div>

		<fieldset>
			<legend>Programs</legend>

			{#each attribute.programs as program, index}
				<div role="group">
					<span class="program-index">{index + 1}</span>
					<input
						type="text"
						value={program}
						oninput={(e) => handleProgramChange(index, e)}
						placeholder="Program name"
					/>
					<button type="button" class="secondary" onclick={() => removeProgram(index)}>🚫</button>
				</div>
			{/each}

			<button type="button" class="outline" onclick={addProgram}>➕</button>
		</fieldset>
	</form>
</article>

<style>
	.program-index {
		display: flex;
		width: 2em;
		font-weight: bold;
		align-items: center;
		justify-content: center;
	}
</style>
